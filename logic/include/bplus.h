#ifndef BPLUS_H
#define BPLUS_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// TODO: how do we want to manage this order?
#define BPLUS_ORDER     18

// TODO: what type of key do we want to store?
typedef uint64_t BplusKey;
typedef void*   BplusData;

typedef struct BplusItem {

    BplusKey key;
    BplusData value;

} BplusItem;

typedef struct BplusNode {

    size_t length;
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

// TODO:
// 06/11/2018 -- 22:36 - target functions

extern BplusTree *bplus_tree_new (void);
extern void bplus_tree_destroy (BplusTree *);
// extern void bplus_tree_destroy_each (BplusTree *, BplusForEach *foreach, void *argument);

extern void bplus_tree_insert (BplusTree const *tree, const BplusKey key, const BplusData value);
extern BplusData bplus_tree_remove (BplusTree const *tree, const BplusKey key);
extern BplusData bplus_tree_remove_first (BplusTree const *tree);
extern void bplus_tree_remove_value (BplusTree const *tree, const BplusKey key, const BplusData value);
extern BplusData bplus_tree_get (BplusTree const *tree, const BplusKey key);

#endif