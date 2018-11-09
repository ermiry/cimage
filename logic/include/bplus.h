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

struct BplusTree;

typedef struct BplusItem {

    BplusKey key;
    BplusData value;

} BplusItem;

typedef void (BplusForeachItemFunc) (struct BplusTree *tree,
    BplusItem *item, void *arg);

#define bplus_key_gt(bplus_tree, k1, k2)    ((k1) > (k2))
#define bplus_key_lt(bplus_tree, k1, k2)    ((k1) < (k2))
#define bplus_key_gte(bplus_tree, k1, k2)   ((k1) >= (k2))
#define bplus_key_lte(bplus_tree, k1, k2)   ((k1) <= (k2))
#define bplus_key_eq(bplus_tree, k1, k2)    ((k1) == (k2))
#define bplus_key_neq(bplus_tree, k1, k2)   ((k1) != (k2))

#define bplus_key_at(node, index)     (((BplusNode*) node)->items[(index)].key)
#define bplus_key_first(node)         bplus_key_at(node, 0)
#define bplus_key_last(node)          bplus_key_at(node, (node)->length - 1)

#define bplus_value_at(node, index)   (((BplusNode*) node)->items[(index)].value)
#define bplus_value_first(node)       bplus_value_at(node, 0)
#define bplus_value_last(node)        bplus_value_at(node, (node)->length - 1)

typedef struct BplusNode {

    size_t length;
    BplusItem items[BPLUS_ORDER];

    bool leaf;
    struct BplusNode *parent;

} BplusNode;

typedef void (BplusForeachNodeFunc) (struct BplusTree *tree, BplusNode *node, void *arg);

#define bplus_node_at(node, index)      ((BplusNode*) bplus_value_at(node, index))
#define bplus_node_first(node)          ((BplusNode*) bplus_value_first(node))
#define bplus_node_last(node)           ((BplusNode*) bplus_value_last(node))

typedef struct BplusLeaf {

    BplusNode node;

    struct BplusLeaf *left;
    struct BplusLeaf *right;

} BplusLeaf;

typedef struct BplusPath {

    size_t length;
    size_t index[16];   // TODO: where do we get the correct value for this?
    BplusNode *leaf;

} BplusPath;

typedef struct BplusIterator {

    BplusItem *item;
    BplusLeaf *leaf;

    BplusLeaf *leaf_from;
    BplusItem *item_from;
    BplusLeaf *leaf_to;
    BplusItem *item_to;

} BplusIterator;

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

extern BplusTree *bplus_tree_new (void);
extern void bplus_tree_destroy (BplusTree *);
// extern void bplus_tree_destroy_each (BplusTree *, BplusForEach *foreach, void *argument);

extern void bplus_tree_insert (BplusTree *tree, const BplusKey key, const BplusData value);
extern BplusData bplus_tree_remove (BplusTree *tree, const BplusKey key);
extern BplusData bplus_tree_remove_first (BplusTree *tree);
extern void bplus_tree_remove_value (BplusTree *tree, const BplusKey key, const BplusData value);

extern BplusData bplus_tree_get_node_data (BplusTree *tree, const BplusKey key);
extern BplusData bplus_tree_get_first_data (BplusTree *tree);
extern BplusData bplus_tree_get_nth_data (BplusTree *tree, const size_t pos);

extern void bplus_tree_print_stats (BplusTree const *tree);

#endif