#ifndef BPLUS_H
#define BPLUS_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// TODO: how do we want to manage this order?
#define BPLUS_ORDER     10

// TODO: what type of key do we want to store?
typedef uint64_t BplusKey;
typedef void*   BplusData;

typedef struct BplusItem {

    BplusKey key;
    BplusData value;

} BplusItem;

typedef struct BplusNode {

    size_t lenght;
    BplusItem items[BPLUS_ORDER];

    bool leaf;
    BplusNode *parent;

} BplusNode;

typedef struct BplusLeaf {

    BplusNode node;

    struct BplusLeaf *left;
    struct BplusLeaf *right;

} BplusLeaf;

typedef struct BplusTree {

    BplusNode *root;
    
    BplusLeaf *first;
    BplusLeaf *last;

    size_t height;

    // TODO: allow duplicated keys?

    // b+ tree stats
    size_t node_count;
    size_t leaf_count;
    size_t underflow_count;     // TODO: what are these?
    size_t overflow_count;

} BplusTree;

// TODO: where do we want to use this?
typedef struct BplusPath {

    size_t length;
    size_t index[16];   // TODO: where do we get the correct value for this?
    BplusNode *leaf;

} BplusPath;

#endif