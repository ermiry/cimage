#ifndef DLIST_H
#define DLIST_H

#include <stdlib.h>
#include <stdbool.h>

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

} DoubleList;

#define LIST_SIZE(list) ((list)->size)

#define LIST_START(list) ((list)->start)
#define LIST_END(list) ((list)->end)

#define LIST_DATA(element) ((element)->data)
#define LIST_NEXT(element) ((element)->next)

extern DoubleList *dlist_init (void (*destroy)(void *data));
extern void dlist_reset (DoubleList *);
// only gets rid of the List elemenst, but the data is kept
extern void dlist_clean (DoubleList *);
extern void dlist_destroy (DoubleList *);

// Elements
extern bool dlist_insert_after (DoubleList *, ListElement *, void *data);
extern void *dlist_remove_element (DoubleList *, ListElement *);

// Searching
extern void *dlist_search (DoubleList *, void *data);
extern bool dlist_is_in_list (DoubleList *, void *data);
ListElement *dlist_get_ListElement (DoubleList *, void *data);

// FIXME:
// Sorting
// extern ListElement *mergeSort (ListElement *head);

#endif