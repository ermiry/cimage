// Implementation of a simple object pool system using a stack

#include <stdlib.h>
#include <stdio.h>

#include "cengine/collections/pool.h"

Pool *pool_init (void (*destroy)(void *data)) {

    Pool *pool = (Pool *) malloc (sizeof (Pool));

    if (pool != NULL) {
        pool->size = 0;
        pool->top = NULL;
        pool->destroy = destroy;
    }

    return pool;

}

void pool_push (Pool *pool, void *data) {

    if (pool && data) {
        PoolMember *new_member = (PoolMember *) malloc (sizeof (PoolMember));
        if (new_member) {
            new_member->data = data;

            if (POOL_SIZE (pool) == 0) new_member->next = NULL;
            else new_member->next = pool->top;

            pool->top = new_member;
            pool->size++;
        }

        // failed to insert on the pool
        else {
            if (pool->destroy) pool->destroy (data);
            else free (data);
        }
    }

}

void *pool_pop (Pool *pool) {

    if (pool) {
        PoolMember *top = POOL_TOP (pool);

        if (top) {
            void *data = top->data;

            pool->top = top->next;
            pool->size--;

            free (top);

            return data;
        }
    }

    return NULL;

}

void pool_delete (Pool *pool) {

    if (pool) {
        if (POOL_SIZE (pool) > 0) {
            void *data = NULL;
            while (pool->size > 0) {
                data = pool_pop (pool);
                if (data) {
                    if (pool->destroy) pool->destroy (data);
                    else free (data);
                }
            }
        }

        free (pool);
    }

}