#include "bplus.h"

#include <string.h>     // for memcpy

#pragma region BPLUS NODE

#define bplus_value_at(node, index) (((BplusNode *) node)->items[(index)].value)
#define bplus_node_at(node, index) ((BplusNode *) bplus_value_at (node, index))

void bplus_node_init (BplusNode *node, bool leaf) {

    if (node) {
        node->parent = NULL;
        node->leaf = leaf;

        node->length = 0;
    }

}

BplusNode *bplus_node_new (BplusTree *tree) {

    if (tree) {
        BplusNode *node = (BplusNode *) malloc (sizeof (BplusNode));

        bplus_node_init (node, false);
        tree->node_count++;
        
        return node;
    }

    else return NULL;

}

BplusNode *bplus_node_new_right (BplusTree *tree, BplusNode *node_left) {

    if (tree && node_left) {
        BplusNode *right = NULL;

        if (node_left->leaf)
            right = (BplusNode *) bplus_leaf_new_right (tree, (BplusLeaf *) node_left);
        else right = bplus_node_new (tree);

        right->parent = node_left->parent;

        return right;
    }

    else return NULL;

}

// FIXME: free the node
void bplus_node_destroy (BplusTree *tree, BplusNode *node) {

    if (tree && node) {
        if (node->leaf) bplus_leaf_destroy (tree, (BplusLeaf *) node);
        else {
            // FIXME: free the node
            tree->node_count--;
        }
    }

}

void bplus_node_move_and_resize_data (BplusTree const *tree, BplusNode *node, 
    const size_t index_to, const size_t index_from) {

    if (node && index_from <= node->length) {
        int64_t const resize_length = index_to - index_from;

        if (resize_length == 0) return;

        int64_t const move_length = node->length - index_from;
        if (move_length > 0)
            memmove (node->items + index_to, node->items + index_from, move_length * sizeof (BplusItem));

        if (resize_length > 0) node->length += resize_length;

        else if (resize_length < 0) node->length -= -resize_length;
    }

}

void bplus_node_insert_at (BplusTree const *tree, BplusNode *node, const size_t index, 
    const size_t length, BplusItem const * const items) {

    if (node && (index <= node->length) && (node->length + length <= BPLUS_ORDER)) {
        bplus_node_move_and_resize_data (tree, node, index + length, index);
        memcpy (node->items + index, items, length * sizeof (BplusItem));

        if (node->leaf == false) 
            for (size_t i = index; i < index + length; ++i) 
                bplus_node_at (node, i)->parent = node;
        
    }
}

#pragma endregion

#pragma region BPLUS LEAF

BplusLeaf *bplus_leaf_new (BplusTree *tree) {

    if (tree) {
        BplusLeaf *leaf = (BplusLeaf *) malloc (sizeof (BplusLeaf));

        bplus_node_init (&leaf->node, true);
        leaf->left = NULL;
        leaf->right = NULL;

        tree->node_count++;
        tree->leaf_count++;

        return leaf;
    }

    else return NULL;

}

BplusLeaf *bplus_leaf_new_right (BplusTree *tree, BplusLeaf *leaf_left) {

    if (tree && leaf_left) {
        BplusLeaf *leaf_right = bplus_leaf_new (tree);

        leaf_right->left = leaf_left;
        leaf_right->right = leaf_left->right;

        leaf_left->right = leaf_right;

        if (leaf_right->right != NULL)
            leaf_right->right->left = leaf_right;

        if (tree->last == leaf_left) tree->last = leaf_right;

        return leaf_right;
    }

    else return NULL;

}

// FIXME: free the data
void bplus_leaf_destroy (BplusTree *tree, BplusLeaf *leaf) {

    if (tree && leaf) {
        if (leaf->left) leaf->left->right = leaf->right;

        if (leaf->right) leaf->right->left = leaf->left;

        if (tree->first == leaf) tree->first = leaf->right;

        if (tree->last == leaf) tree->last = leaf->left;

        tree->node_count--;
        tree->leaf_count--;

        // FIXME: free the data
        // g_slice_free(BplusLeaf, leaf);
    }

}

#pragma endregion

#pragma region BPLUS SEARCH

static size_t bplus_node_get_key_index (BplusTree const *tree, BplusNode const *node, 
    const BplusKey key) {

    if (tree && node) {}

}

#pragma endregion

#pragma region BPLUS REBALANCE

void bplus_rebalance_propagate (BplusTree const *tree, BplusPath *path) {

    if (tree && path) {
        size_t index;
        BplusKey key;
        BplusNode *node = path->leaf;
        for (size_t i = 1; i < path->length; ++i) {
            index = path->index[i];
            key = bplus_key_first (node);

            if (bplus_key_gt (tree, bplus_key_at (node->parent, index), key))
                bplus_key_at (node->parent, index) = key;

            else break;

            node = node->parent;
        }
    }

}

static int64_t bplus_node_get_rebalance_amount (BplusTree const *tree, BplusNode const *node_left, 
    BplusNode const *node_right) {

    if (tree && node_left && node_right) {
        const size_t total = node_left->length + node_right->length;

        // nodes cna be merged without overfilling
        if (total <= (BPLUS_ORDER / 3)) return -node_right->length;

        // data can be balanced over the 2 without overfilling
        else if (total < (BPLUS_ORDER * 5 / 3))
            return node_left->length - (total + 1) / 2;

        return 0;
    }

    return 0;

}

static size_t bplus_rebalance_data (BplusTree const *tree, BplusNode *node_left, 
    BplusNode *node_right) {

    if (tree && node_right && node_left) {
        const int64_t amount = bplus_node_get_rebalance_amount (tree, node_left, node_right);
        if (amount <= 0 || amount < node_left->length) return 0;
        if (amount >= 0 || -amount <= node_right->length) return 0;

        if (amount > 0) {
            const size_t index = node_left->length - amount;
            BplusItem const *items = node_left->items + index;

            bplus_node_insert_at (tree, node_right, 0, amount, items);
            bplus_node_remove_at (tree, node_left, index, amount);
        }

        else if (amount < 0) {
            const size_t index = node_left->length;
            BplusItem const *items = node_right->items;

            bplus_node_insert_at (tree, node_left, index, -amount, items);
            bplus_node_remove_at (tree, node_right, 0, -amount);
        }

        return 0;
    }

}

static void bplus_rebalance_split_node (BplusTree const *tree, BplusNode *node_left, 
    const size_t index) {

    if (tree && node_left) {
        BplusNode *const node_right = bplus_node_new_right (tree, node_left);
        bplus_rebalance_data (tree, node_left, node_right);

        BplusItem const item = { .key = bplus_key_first (node_right), .value = node_right };
        bplus_node_insert_at (tree,node_left->parent, index + 1, 1, &item);
    }

}

static void bplus_rebalance_new_root (BplusTree const *tree) {

    if (tree) {
        BplusNode *const node = tree->root;
        BplusNode *const root = bplus_node_new (tree);

        bplus_key_first (root) = bplus_key_first (node);
        bplus_value_first (root) = node;

        root->length = 1;
        root->parent = root;
        tree->root = root;
        tree->height++;
    }

}

// FIXME:
static bool bplus_node_find_merge_candidate (BplusTree const *tree, const size_t index, 
    BplusNode *node, BplusNode **node_left, BplusNode **node_right) {

}

static int bplus_rebalance_try_merge (BplusTree const *tree, BplusNode *node, const size_t index) {

    if (tree && node) {
        BplusNode *node_left = NULL, *node_right = NULL;

        if (!bplus_node_find_merge_candidate (tree, index, node, &node_left, &node_right));
            return 0;

        const size_t index_right = (node == node_left) ? index + 1 : index;
        bplus_rebalance_data (tree, node_left, node_right);

        if (node_right->length == 0) {
            bplus_node_remove_at (tree, node->parent, index_right, 1);
            bplus_node_destroy (tree, node_right);
        }

        else bplus_key_at (node->parent, index_right) = bplus_key_first (node_right);

        return 1;
    }

    else return 0;

}

static void bplus_rebalance_shrink_tree (BplusTree const *tree) {

    if (tree && tree->root) {
        size_t i = 0;
        for (; i < tree->height; ++i) {
            if (tree->root->length != 1) break;
            if (tree->root->leaf) break;

            BplusNode *node = tree->root;
            tree->root = bplus_node_first (tree->root);
            tree->root->parent = NULL;
            node->length = 0;
            bplus_node_destroy (tree, node);
        }

        tree->height -= i;
    }

}

void bplus_rebalance_overfilled (BplusTree const *tree, BplusPath const *path) {

    if (tree && path) { 
        BplusNode *node = path->leaf;
        for (size_t i = 1; i < path->length; ++i) {
            if (!bplus_node_overfilled (node)) break;

            const size_t index = path->index[i] ;
            if (!bplus_rebalance_try_merge (tree, node, index))
                bplus_rebalance_split_node (tree, node, index);

                node = node->parent;
        }

        if (bplus_node_overfilled (node)) {
            bplus_rebalance_new_root (tree);
            bplus_rebalance_split_node (tree, node, 0);
        }
    }

}

void bplus_rebalance_underfilled (BplusTree const *tree, BplusPath const *path) {

    if (tree && path) {
        BplusNode *node = path->leaf;
        for (size_t i = 1; i < path->length; ++i) {
            if (!bplus_node_underfilled (node)) break;

            const size_t index = path->index[i];
            if (!bplus_rebalance_try_merge (tree, node, index) && (node->length == 0)) {
                bplus_node_remove_at (tree, node->parent, index, 1);
                bplus_node_destroy (tree, node);
            }

            node = node->parent;
        }

        bplus_rebalance_shrink_tree (tree);
    }

}

#pragma endregion

#pragma region BPLUS INSERT

#define bplus_key_at(node, index)      (((BplusNode *) (node))->items[(index)].key)
#define bplus_value_at(node, index)    (((BplusNode *) (node))->items[(index)].value)

void bplus_leaf_insert_at (BplusTree const *tree, BplusNode *node, const size_t index, 
    const BplusKey key, const BplusData value) {

    if (node && index <= node->length) {
        bplus_node_move_and_resize_data (tree, node, index + 1, index);
        bplus_key_at (node, index) = key;
        bplus_value_at (node, index) = value;
    }
    
}

void bplus_node_insert_at (BplusTree const *tree, BplusNode *node, const size_t index,
    const size_t length, BplusItem const * const items) {

    if (node && (index <= node->length) && (node->length + length <= BPLUS_ORDER)) {
        bplus_node_move_and_resize_data (tree, node, index + length, index);
        memcpy (node->items + index, items, length * sizeof (BplusItem));

        if (node->leaf == false)  
            for (size_t i = index; i < index + length; i++)
                bplus_node_at (node, i)->parent = node;
    }

}

void bplus_tree_insert (BplusTree const *tree, const BplusKey key, const BplusData value) {

    BplusPath path;
    bplus_tree_get_path_to_key (tree, key, &path);

    size_t index = path.index[0];
    BplusNode *node = (BplusNode *) path.leaf;

    if ((index < node->length) && bplus_key_lte (tree, bplus_key_at (node, index), key))
        index++;

    bplus_leaf_insert_at (tree, node, index, key, value);
    if (index == 0) bplus_rebalance_propagate (tree, &path);
    if (bplus_node_overfilled (node)) bplus_rebalance_overfilled (tree, &path);

}

#pragma endregion

#pragma region BPLUS REMOVE

void bplus_node_remove_at (BplusTree const *tree, BplusNode *node, const size_t index, 
    const size_t length) {

    if (node && (index < node->length) && (index + length <= node->length)) 
        bplus_node_move_and_resize_data (tree, node, index, index + length);

}

BplusData bplus_tree_remove_first (BplusTree const *tree) {

    BplusPath path = { .leaf = (BplusNode *) tree->first };
    BplusNode *node = (BplusNode *) path.leaf;
    const size_t index = path.index[0];

    BplusData value = bplus_value_at (node, 0);
    bplus_node_remove_at (tree, node, index, 1);

    if (index == 0) bplus_rebalance_propagate (tree, &path);

    if (bplus_node_underfilled (node)) bplus_rebalance_underfilled (tree, &path);

    return value;

}

BplusData bplus_tree_remove (BplusTree const *tree, const BplusKey key) {

    BplusPath path;
    bplus_tree_get_path_to_key (tree, key, &path);

    const size_t index = path.index[0];
    BplusNode *node = (BplusNode *) path.leaf;

    if (bplus_key_eq (tree, bplus_key_at (node, index), key)) {
        BplusData value = bplus_value_at (node, index);
        bplus_node_remove_at (tree, node, index, 1);

        if (index == 0) bplus_rebalance_propagate (tree, &path);
        
        if (bplus_node_underfilled (node)) bplus_rebalance_underfilled (tree, &path);

        return value;
    }

    else return NULL;

}

#pragma endregion

#pragma region BPLUS TREE

// TODO: more stats
// inits a new bplus tree
BplusTree *bplus_tree_new (void) {

    BplusTree *tree = (BplusTree *) malloc (sizeof (BplusTree));

    tree->first = bplus_leaf_new (tree);
    tree->last = tree->first;
    tree->root = (BplusNode *) tree->first;

    tree->height = 1;

    tree->node_count = 1;
    tree->leaf_count = 0;

    // TODO: stats
    /* size_t underflow_count;
    size_t overflow_count; */

    return tree;

}

#pragma endregion