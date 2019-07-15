// /**********************************
//  * @author      Johan Hanssen Seferidis
//  * License:     MIT
//  **********************************/

// #ifndef _CERVER_THPOOL_H_
// #define _CERVER_THPOOL_H_

// #include <pthread.h>

// #include "cengine/types/string.h"

// struct threadpool;

// // binary semaphore
// typedef struct bsem {

// 	pthread_mutex_t mutex;
// 	pthread_cond_t   cond;
// 	int v;

// } bsem;

// typedef struct job {
	
// 	struct job *prev;                   /* pointer to previous job   */
// 	void (*function)(void* arg);        /* function pointer          */
// 	void *arg;                          /* function's argument       */

// } job;

// typedef struct jobqueue {

// 	pthread_mutex_t rwmutex;             /* used for queue r/w access */
// 	job  *front;                         /* pointer to front of queue */
// 	job  *rear;                          /* pointer to rear  of queue */
// 	bsem *has_jobs;                      /* flag as binary semaphore  */
// 	int   len;                           /* number of jobs in queue   */

// } jobqueue;

// typedef struct thread {

// 	int id;                        		 /* friendly id               */
// 	pthread_t pthread;                   /* pointer to actual thread  */
// 	struct threadpool *thpool_p;         /* access to thpool          */

// } thread;

// typedef struct threadpool {

// 	String *name;

// 	thread **threads;                    /* pointer to threads        */
// 	volatile int num_threads_alive;      /* threads currently alive   */
// 	volatile int num_threads_working;    /* threads currently working */
// 	pthread_mutex_t  thcount_lock;       /* used for thread count etc */
// 	pthread_cond_t  threads_all_idle;    /* signal to thpool_wait     */
// 	jobqueue  *job_queue;                 /* job queue                 */

// } threadpool;

// //  * Initializes a threadpool. This function will not return untill all
// //  * threads have initialized successfully.
// extern threadpool *thpool_create (const char *name, unsigned int num_threads);

// //  Takes an action and its argument and adds it to the threadpool's job queue.
// //  * If you want to add to work a function with more than one arguments then
// //  * a way to implement this is by passing a pointer to a structure.
// extern int thpool_add_work (threadpool *thpool_p, void (*function_p)(void*), void* arg_p);

// //  * Will wait for all jobs - both queued and currently running to finish.
// //  * Once the queue is empty and all work has completed, the calling thread
// //  * (probably the main program) will continue.
// //  *
// //  * Smart polling is used in wait. The polling is initially 0 - meaning that
// //  * there is virtually no polling at all. If after 1 seconds the threads
// //  * haven't finished, the polling interval starts growing exponentially
// //  * untill it reaches max_secs seconds. Then it jumps down to a maximum polling
// //  * interval assuming that heavy processing is being used in the threadpool.
// extern void thpool_wait (threadpool *thpool_p);

// //  * The threads will be paused no matter if they are idle or working.
// //  * The threads return to their previous states once thpool_resume
// //  * is called.
// //  *
// //  * While the thread is being paused, new work can be added.
// extern void thpool_pause (threadpool *thpool_p);

// // Unpauses all threads if they are paused
// extern void thpool_resume (threadpool *thpool_p);

// // gets how many threads are active
// extern int thpool_num_threads_working (threadpool *thpool_p);

// //  * This will wait for the currently active threads to finish and then 'kill'
// //  * the whole threadpool to free up memory.
// extern void thpool_destroy (threadpool *thpool_p);

// #endif

#ifndef _CERVER_THPOOL_H_
#define _CERVER_THPOOL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* =================================== API ======================================= */


typedef struct thpool_* threadpool;


/**
 * @brief  Initialize threadpool
 *
 * Initializes a threadpool. This function will not return untill all
 * threads have initialized successfully.
 *
 * @example
 *
 *    ..
 *    threadpool thpool;                     //First we declare a threadpool
 *    thpool = thpool_init(4);               //then we initialize it to 4 threads
 *    ..
 *
 * @param  num_threads   number of threads to be created in the threadpool
 * @return threadpool    created threadpool on LOG_SUCCESS,
 *                       NULL on error
 */
extern threadpool thpool_init(int num_threads);


/**
 * @brief Add work to the job queue
 *
 * Takes an action and its argument and adds it to the threadpool's job queue.
 * If you want to add to work a function with more than one arguments then
 * a way to implement this is by passing a pointer to a structure.
 *
 * NOTICE: You have to cast both the function and argument to not get warnings.
 *
 * @example
 *
 *    void print_num(int num){
 *       printf("%d\n", num);
 *    }
 *
 *    int main() {
 *       ..
 *       int a = 10;
 *       thpool_add_work(thpool, (void*)print_num, (void*)a);
 *       ..
 *    }
 *
 * @param  threadpool    threadpool to which the work will be added
 * @param  function_p    pointer to function to add as work
 * @param  arg_p         pointer to an argument
 * @return 0 on successs, -1 otherwise.
 */
extern int thpool_add_work(threadpool, void (*function_p)(void*), void* arg_p);


/**
 * @brief Wait for all queued jobs to finish
 *
 * Will wait for all jobs - both queued and currently running to finish.
 * Once the queue is empty and all work has completed, the calling thread
 * (probably the main program) will continue.
 *
 * Smart polling is used in wait. The polling is initially 0 - meaning that
 * there is virtually no polling at all. If after 1 seconds the threads
 * haven't finished, the polling interval starts growing exponentially
 * untill it reaches max_secs seconds. Then it jumps down to a maximum polling
 * interval assuming that heavy processing is being used in the threadpool.
 *
 * @example
 *
 *    ..
 *    threadpool thpool = thpool_init(4);
 *    ..
 *    // Add a bunch of work
 *    ..
 *    thpool_wait(thpool);
 *    puts("All added work has finished");
 *    ..
 *
 * @param threadpool     the threadpool to wait for
 * @return nothing
 */
extern void thpool_wait(threadpool);


/**
 * @brief Pauses all threads immediately
 *
 * The threads will be paused no matter if they are idle or working.
 * The threads return to their previous states once thpool_resume
 * is called.
 *
 * While the thread is being paused, new work can be added.
 *
 * @example
 *
 *    threadpool thpool = thpool_init(4);
 *    thpool_pause(thpool);
 *    ..
 *    // Add a bunch of work
 *    ..
 *    thpool_resume(thpool); // Let the threads start their magic
 *
 * @param threadpool    the threadpool where the threads should be paused
 * @return nothing
 */
extern void thpool_pause(threadpool);


/**
 * @brief Unpauses all threads if they are paused
 *
 * @example
 *    ..
 *    thpool_pause(thpool);
 *    sleep(10);              // Delay execution 10 seconds
 *    thpool_resume(thpool);
 *    ..
 *
 * @param threadpool     the threadpool where the threads should be unpaused
 * @return nothing
 */
extern void thpool_resume(threadpool);


/**
 * @brief Destroy the threadpool
 *
 * This will wait for the currently active threads to finish and then 'kill'
 * the whole threadpool to free up memory.
 *
 * @example
 * int main() {
 *    threadpool thpool1 = thpool_init(2);
 *    threadpool thpool2 = thpool_init(2);
 *    ..
 *    thpool_destroy(thpool1);
 *    ..
 *    return 0;
 * }
 *
 * @param threadpool     the threadpool to destroy
 * @return nothing
 */
extern void thpool_destroy(threadpool);


/**
 * @brief Show currently working threads
 *
 * Working threads are the threads that are performing work (not idle).
 *
 * @example
 * int main() {
 *    threadpool thpool1 = thpool_init(2);
 *    threadpool thpool2 = thpool_init(2);
 *    ..
 *    printf("Working threads: %d\n", thpool_num_threads_working(thpool1));
 *    ..
 *    return 0;
 * }
 *
 * @param threadpool     the threadpool of interest
 * @return integer       number of threads working
 */
extern int thpool_num_threads_working(threadpool);


#ifdef __cplusplus
}
#endif

#endif