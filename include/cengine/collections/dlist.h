#ifndef _COLLECTIONS_DLIST_H_
#define _COLLECTIONS_DLIST_H_

#include <pthread.h>

typedef struct ListElement {

	struct ListElement *prev;
	void *data;
	struct ListElement *next;

} ListElement;

typedef struct DoubleList {

	size_t size;

	ListElement *start;
	ListElement *end;

	void (*destroy)(void *data);
	int (*compare)(const void *one, const void *two);

	pthread_mutex_t *mutex;

} DoubleList;

#define dlist_size(list) ((list)->size)

#define dlist_start(list) ((list)->start)
#define dlist_end(list) ((list)->end)

#define dlist_element_data(element) ((element)->data)
#define dlist_element_next(element) ((element)->next)

extern void dlist_delete (void *dlist_ptr);

// sets a list compare function
// compare must return -1 if one < two, must return 0 if they are equal, and must return 1 if one > two
extern void dlist_set_compare (DoubleList *list, int (*compare)(const void *one, const void *two));

// sets list destroy function
extern void dlist_set_destroy (DoubleList *list, void (*destroy)(void *data));

// creates a new double list (double linked list)
// destroy is the method used to free up the data, NULL to use the default free
// compare must return -1 if one < two, must return 0 if they are equal, and must return 1 if one > two
extern DoubleList *dlist_init (void (*destroy)(void *data),
	int (*compare)(const void *one, const void *two));

// destroys all of the dlist's elements and their data but keeps the dlist
extern void dlist_reset (DoubleList *);

// only gets rid of the list elements, but the data is kept
// this is usefull if another dlist or structure points to the same data
extern void dlist_clean (DoubleList *);

/*** insert ***/

// inserts the data in the double list BEFORE the specified element
// if element == NULL, data will be inserted at the start of the list
// returns 0 on success, 1 on error
extern int dlist_insert_before (DoubleList *dlist, ListElement *element, void *data);

// inserts the data in the double list AFTER the specified element
// if element == NULL, data will be inserted at the start of the list
// returns 0 on success, 1 on error
extern int dlist_insert_after (DoubleList *dlist, ListElement *element, void *data);

// inserts the data in the double list in the specified pos (0 indexed)
// if the pos is greater than the current size, it will be added at the end
// returns 0 on success, 1 on error
extern int dlist_insert_at (DoubleList *dlist, void *data, unsigned int pos);

/*** remove ***/

// finds the data using the query and the list comparator and the removes it from the list
// and returns the list element's data
// option to pass a custom compare method for searching, if NULL, dlist's compare method will be used
extern void *dlist_remove (DoubleList *dlist, void *query, int (*compare)(const void *one, const void *two));

// removes the dlist element from the dlist and returns the data
// NULL for the start of the list
extern void *dlist_remove_element (DoubleList *dlist, ListElement *element);

// removes the dlist element from the dlist at the specified index 
// returns the data or NULL if index was invalid
extern void *dlist_remove_at (DoubleList *dlist, unsigned int idx);

/*** Traversing --- Searching ***/

// traverses the dlist and for each element, calls the method by passing the list element data and the method args as both arguments
// this method is thread safe
// returns 0 on success, 1 on error
extern int dlist_traverse (DoubleList *dlist, 
	void (*method)(void *list_element_data, void *method_args), void *method_args);

// uses the list comparator to search using the data as the query
// option to pass a custom compare method for searching, if NULL, dlist's compare method will be used
// returns the double list's element data
extern void *dlist_search (DoubleList *dlist, void *data, int (*compare)(const void *one, const void *two));

// searches the dlist and returns the dlist element associated with the data
// option to pass a custom compare method for searching
extern ListElement *dlist_get_element (DoubleList *dlist, void *data, 
	int (*compare)(const void *one, const void *two));

// traverses the dlist and returns the list element at the specified index
extern ListElement *dlist_get_element_at (DoubleList *dlist, unsigned int idx);

// traverses the dlist and returns the data of the list element at the specified index
extern void *dlist_get_at (DoubleList *dlist, unsigned int idx);

/*** Sorting ***/

// uses merge sort to sort the list using the comparator
// option to pass a custom compare method for searching, if NULL, dlist's compare method will be used
// return 0 on succes 1 on error
extern int dlist_sort (DoubleList *dlist, int (*compare)(const void *one, const void *two));

#endif