#include "bplus.h"

#include <string.h>     // for memcpy

#pragma region BPLUS NODE

#define bplus_value_at(node, index) (((BplusNode *) node)->items[(index)].value)
#define bplus_node_at(node, index) ((BplusNode *) bplus_value_at (node, index))

void bplus_node_init (BplusNode *node, bool leaf) {

    if (node) {
        node->parent = NULL;
        node->leaf = leaf;

        node->lenght = 0;
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

// TODO: 
void bplus_node_move_and_resize_data (BplusTree const *tree, BplusNode *node, 
    const size_t index_to, const size_t index_from) {

}

void bplus_node_insert_at (BplusTree const *tree, BplusNode *node, const size_t index, 
    const size_t lenght, BplusItem const * const items) {

    if (node && (index <= node->lenght) && (node->lenght + lenght <= BPLUS_ORDER)) {
        bplus_node_move_and_resize_data (tree, node, index + lenght, index);
        memcpy (node->items + index, items, lenght * sizeof (BplusItem));

        if (node->leaf == false) 
            for (size_t i = index; i < index + lenght; ++i) 
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

#pragma region BPLUS TREE

// TODO: more stats
// inits a new bplus tree
BplusTree *bplus_new (void) {

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