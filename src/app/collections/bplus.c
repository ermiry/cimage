#include "app/collections/bplus.h"

#include <stdio.h>
#include <string.h>

#pragma region BPLUS NODE

void bplus_node_insert_at (BplusTree const *tree, BplusNode *node, const size_t index,
    const size_t length, BplusItem const * const items);

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

BplusLeaf *bplus_leaf_new_right (BplusTree *tree, BplusLeaf *leaf_left);

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

void bplus_leaf_destroy (BplusTree *tree, BplusLeaf *leaf);

// TODO: 12/11/2018 -- where do we free the node data?
void bplus_node_destroy (BplusTree *tree, BplusNode *node) {

    if (tree && node) {
        if (node->leaf) bplus_leaf_destroy (tree, (BplusLeaf *) node);
        else {
            free (node);
            tree->node_count--;
        }
    }

}

void bplus_node_move_and_resize_data (BplusTree const *tree, BplusNode *node, 
    const size_t index_to, const size_t index_from) {

    if (node && index_from <= node->length) {
        const int64_t resize_length = index_to - index_from;

        if (resize_length == 0) return;

        const int64_t move_length = node->length - index_from;
        if (move_length > 0)
            memmove (node->items + index_to, node->items + index_from, move_length * sizeof (BplusItem));

        if (resize_length > 0) node->length += resize_length;

        else if (resize_length < 0) node->length -= -resize_length;
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

// TODO: 12/11/2018 -- where do we free the node data?
void bplus_leaf_destroy (BplusTree *tree, BplusLeaf *leaf) {

    if (tree && leaf) {
        if (leaf->left) leaf->left->right = leaf->right;

        if (leaf->right) leaf->right->left = leaf->left;

        if (tree->first == leaf) tree->first = leaf->right;

        if (tree->last == leaf) tree->last = leaf->left;

        tree->node_count--;
        tree->leaf_count--;

        free (leaf);
    }

}

#pragma endregion

#pragma region BPLUS SEARCH

#define bplus_node_get_key_index_op(operator, tree, node, key)              \
    do {                                                                    \
        size_t index = 1;                                                   \
        while (index < (node)->length) {                                    \
            if (operator ((tree), bplus_key_at (node, index), (key)))       \
                break;                                                      \
            ++index;                                                        \
        }                                                                   \
                                                                            \
        return --index;                                                     \
    }                                                                       \
    while (0)

// 64 is the double of the original BPLUS_ORDER (32)
#define bplus_tree_get_path_to_key_op(operator, tree, key, path)                   \
    do {                                                                           \
        if (__builtin_expect (((tree)->root->length == 0), 0)) {                   \
            (path)->length   = 1;                                                  \
            (path)->index[0] = 0;                                                  \
            (path)->leaf     = tree->root;                                         \
            return;                                                                \
        }                                                                          \
                                                                                   \
        BplusNode const *node   = (tree)->root;                                    \
        size_t const     length = (tree)->height;                                  \
        for (size_t i = length; i > 0; --i) {                                      \
            for (int i = 0; i < BPLUS_ORDER * sizeof (*(node)->items) / 64; ++i)   \
                __builtin_prefetch (node->items + i * 64 / sizeof(*node->items));  \
                                                                                   \
            size_t const index = operator ((tree), node, (key));                 \
            (path)->index[i - 1] = index;                                          \
                                                                                   \
            if (i == 1)                                                            \
                break;                                                             \
                                                                                   \
            node = bplus_node_at (node, index);                                    \
        }                                                                          \
                                                                                   \
        (path)->length = length;                                                   \
        (path)->leaf   = (BplusNode*) node;                                        \
    }                                                                              \
    while (0)

size_t bplus_node_get_key_index (BplusTree const *tree, BplusNode const *node, 
    const BplusKey key) {

    if (tree && node) bplus_node_get_key_index_op (bplus_key_gt, tree, node, key);
    else return 0;

}

size_t bplus_node_get_key_index_before (BplusTree const *tree, BplusNode const *node,
    const BplusKey key) {

    if (tree && node) bplus_node_get_key_index_op (bplus_key_gte, tree, node, key);
    else return 0;

}

// TODO: check the original function --> assert condition
void bplus_tree_get_path_to_key (BplusTree const *tree, const BplusKey key, BplusPath *path) {

    if (tree) 
        if (tree->height < sizeof (path->index) / sizeof (*(path)->index))
            bplus_tree_get_path_to_key_op (bplus_node_get_key_index, tree, key, path);

}

// TODO: check the original function --> assert condition
void bplus_tree_get_path_to_key_before (BplusTree const *tree, const BplusKey key,
    BplusPath *path) {

    if (tree) 
        if (tree->height < sizeof (path->index) / sizeof (*(path)->index))
            bplus_tree_get_path_to_key_op (bplus_node_get_key_index_before, tree, key, path);

}

// TODO: check the original function --> assert condition
void bplus_tree_get_paths_to_key_range (BplusTree const *tree, BplusKey key_from, BplusKey key_to,
    BplusPath *path_from, BplusPath *path_to) {

    if (tree) {
        // TODO: assert conditions

        if (bplus_key_gt (tree, key_from, key_to)) {
            BplusKey temp = key_to;
            key_to = key_from;
            key_from = temp;
        }

        bplus_tree_get_path_to_key (tree, key_to, path_to);
        if (!bplus_key_gt (tree, bplus_key_at (path_to->leaf, path_to->index[0]), key_to))
            path_to->index[0]++;

        bplus_tree_get_path_to_key_before (tree, key_from, path_from);
        if (!bplus_key_gte (tree, bplus_key_at (path_to->leaf, path_from->index[0]), key_from))
            path_from->index[0]++;

        BplusLeaf *leaf = (BplusLeaf *) path_from->leaf;
        if (path_from->index[0] == leaf->node.leaf) {
            if (!leaf->right) {
                path_from->leaf = path_to->leaf;
                path_from->index[0] = path_to->index[0];
            }

            else {
                path_from->leaf = (BplusNode *) leaf->right;
                path_from->index[0] = 0;
            }
        }
    }

}

#pragma endregion

#pragma region BPLUS ITERATOR

BplusIterator *bplus_iterator_new_full (BplusTree *tree, BplusLeaf *leaf_from, 
    BplusItem *item_from, BplusLeaf *leaf_to, BplusItem *item_to) {

    BplusIterator *iterator = (BplusIterator *) malloc (sizeof (BplusIterator));

    if (iterator) {
        iterator->leaf_from = leaf_from;
        iterator->item_from = item_from;
        iterator->leaf_to = leaf_to;
        iterator->item_to = item_to;

        iterator->item = item_from;
        iterator->leaf = leaf_from;

        return iterator;
    }

    return NULL;

}

BplusIterator *bplus_iterator_new_to_last (BplusTree *tree, BplusLeaf *leaf_from, 
    BplusItem *item_from) {

    return bplus_iterator_new_full (tree, leaf_from, item_from, tree->last, 
        tree->last->node.items + tree->last->node.length);

}

BplusIterator *bplus_iterator_new_from_first (BplusTree *tree, BplusLeaf *leaf_to,
    BplusItem *item_to) {

    return bplus_iterator_new_full (tree, tree->first, tree->first->node.items, 
        leaf_to, item_to);

}

BplusIterator *bplus_iterator_new (BplusTree *tree) {

    return bplus_iterator_new_to_last (tree, tree->first, tree->first->node.items);

}

void bplus_iterator_destroy (BplusIterator *iterator) {

    if (iterator) {
        iterator->item = NULL;
        iterator->leaf = NULL;

        iterator->leaf_from = NULL;
        iterator->item_from = NULL;
        iterator->leaf_to = NULL;
        iterator->item_to = NULL;

        free (iterator);
    }

}

bool bplus_iterator_next (BplusIterator *iterator) {

    if (iterator) {
        BplusItem const * const next = iterator->item + 1;
        BplusLeaf const * const leaf = iterator->leaf;

        if (next == iterator->item_to) return false;

        if (next - leaf->node.items < leaf->node.length) ++iterator->item;
        else {
            if (!leaf->right) return false;

            iterator->leaf = leaf->right;
            iterator->item = leaf->right->node.items;
        }

        return true;
    }

    else return false;

}

bool bplus_iterator_previous (BplusIterator *iterator) {

    if (iterator) {
        BplusItem const * const item = iterator->item;
        BplusLeaf const * const leaf = iterator->leaf;

        if (item == iterator->item_from) return false;

        if (item - leaf->node.items == 0) {
            if (!leaf->left) return false;

            iterator->leaf = leaf->left;
            iterator->item = leaf->left->node.items + leaf->left->node.length;
        }

        --iterator->item;

        return true;
    }

    else return false;

}

BplusItem const *bplus_iterator_get_item (BplusIterator const *iterator) {

    if (iterator) {
        if (iterator->item_from == iterator->item_to) return NULL;
        else return iterator->item;
    }

    else return NULL;

}

BplusIterator *bplus_tree_first (BplusTree *tree) {

    if (tree) return bplus_iterator_new (tree);
    else return NULL;

}

BplusIterator *bplus_iterator_from_key (BplusTree *tree, const BplusKey key) {

    if (tree) {
        BplusPath path_from, path_to;
        bplus_tree_get_paths_to_key_range (tree, key, key, &path_from, &path_to);

        return bplus_iterator_new_to_last (tree, (BplusLeaf *) path_from.leaf, 
            path_from.leaf->items + path_from.index[0]);
    }

    else return false;

}

BplusIterator *bplus_iterator_to_key (BplusTree *tree, const BplusKey key) {

    if (tree) {
        BplusPath path_from, path_to;
        bplus_tree_get_paths_to_key_range (tree, key, key, &path_from, &path_to);

        return bplus_iterator_new_from_first (tree, (BplusLeaf *) path_to.leaf,
            path_to.leaf->items + path_to.index[0]);
    }

    else return NULL;

}

BplusIterator *bplus_iterator_for_key (BplusTree *tree, const BplusKey key) {

    if (tree) {
        BplusPath path_from, path_to;
        bplus_tree_get_paths_to_key_range (tree, key, key, &path_from, &path_to);

        return bplus_iterator_new_full (tree, (BplusLeaf *) path_from.leaf, 
            path_from.leaf->items + path_from.index[0], (BplusLeaf *) path_to.leaf,
            path_to.leaf->items + path_to.index[0]);
    }

    else return NULL;

}

BplusIterator *bplus_iterator_for_key_range (BplusTree *tree, const BplusKey key_from,
    const BplusKey key_to) {

    if (tree) {
        BplusPath path_from, path_to;
        bplus_tree_get_paths_to_key_range (tree, key_from, key_to, &path_from, &path_to);

        return bplus_iterator_new_full (tree,
            (BplusLeaf *) path_from.leaf, path_from.leaf->items + path_from.index[0],
            (BplusLeaf *) path_to.leaf, path_to.leaf->items + path_to.index[0]);
    }

    else return NULL;

}


#pragma endregion

#pragma region BPLUS FOREACH

void bplus_foreach_item_in_node (BplusTree *tree, BplusNode *node, 
    BplusForeachItemFunc *foreach, void *arg) {

    if (tree && node) {
        if (!node->leaf)
            for (size_t i = 0; i < node->length; i++)
                bplus_foreach_item_in_node (tree, bplus_node_at (node, i), foreach, arg);

        else
            for (size_t i = 0; i < node->length; i++)
                foreach (tree, node->items + i, arg);
    }
    
}

void bplus_foreach_item_in_tree (BplusTree *tree, BplusForeachItemFunc *foreach, 
    void *arg) {

    if (tree)
        bplus_foreach_item_in_node (tree, tree->root, foreach, arg);

}

void bplus_foreach_node_in_node (BplusTree *tree, BplusNode *node, BplusForeachNodeFunc *foreach,
    void *arg) {

    if (tree && node) {
        if (!node->leaf)
            for (size_t i = 0; i < node->length; ++i)
                bplus_foreach_node_in_node (tree, bplus_node_at (node, i), foreach, arg);

        foreach (tree, node, arg);
    }

}

void bplus_foreach_node_in_tree (BplusTree *tree, BplusForeachNodeFunc *foreach, 
    void *arg) {

    if (tree)
        bplus_foreach_node_in_node (tree, tree->root, foreach, arg);

}

#pragma endregion

#pragma region BPLUS REBALANCE

void bplus_node_remove_at (BplusTree const *tree, BplusNode *node, const size_t index, 
    const size_t length);

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

        // nodes can be merged without overfilling
        if (total <= (BPLUS_ORDER / 3)) return -node_right->length;

        // data can be balanced over the 2 without overfilling
        else if (total < (BPLUS_ORDER * 5 / 3))
            return node_left->length - (total + 1) / 2;

        return 0;
    }

    else return 0;

}

// TODO: 12/11/2018 - check this! not all paths return a valid value?
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

    else return 0;

}

static void bplus_rebalance_split_node (BplusTree *tree, BplusNode *node_left, 
    const size_t index) {

    if (tree && node_left) {
        BplusNode *const node_right = bplus_node_new_right (tree, node_left);
        bplus_rebalance_data (tree, node_left, node_right);

        BplusItem const item = { .key = bplus_key_first (node_right), .value = node_right };
        bplus_node_insert_at (tree, node_left->parent, index + 1, 1, &item);
    }

}

static void bplus_rebalance_new_root (BplusTree *tree) {

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

static bool bplus_node_find_merge_candidate (BplusTree const *tree, const size_t index, 
    BplusNode *node, BplusNode **node_left, BplusNode **node_right) {

    if (tree && node) {
        *node_left  = node;
        *node_right = node;
        if ((node->parent == NULL) || (node->parent->length <= 1)) return false;

        if (index == 0) {
            *node_right = bplus_node_at (node->parent, index + 1);
            if (bplus_node_get_rebalance_amount (tree, node, *node_right) == 0)
                *node_right = node;
        }

        else if (index == node->parent->length - 1) {
            *node_left = bplus_node_at (node->parent, index - 1);
            if (bplus_node_get_rebalance_amount (tree, *node_left, node) == 0)
                *node_left = node;
        }

        else {
            *node_left  = bplus_node_at (node->parent, index - 1);
            *node_right = bplus_node_at (node->parent, index + 1);

            const int64_t merge_amount_right = 
                bplus_node_get_rebalance_amount (tree, node, *node_right);
            const int64_t merge_amount_left = 
                bplus_node_get_rebalance_amount (tree, *node_left, node);

            if (merge_amount_left < 0) {
                if (merge_amount_right >= 0) *node_right = node;
                else if (merge_amount_right < merge_amount_left)
                    *node_right = node;
                else *node_left = node;
            }

            else if (merge_amount_left > 0) {
                if (merge_amount_right == 0) *node_right = node;
                else if (merge_amount_right > merge_amount_left)
                    *node_right = node;
                else *node_left = node;
            }

            else if (merge_amount_right != 0) *node_left = node;
 
            else {
                *node_left  = node;
                *node_right = node;
            }
        }

        return *node_left != *node_right;
    }

    else return false;

}

static int bplus_rebalance_try_merge (BplusTree *tree, BplusNode *node, const size_t index) {

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

static void bplus_rebalance_shrink_tree (BplusTree *tree) {

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

void bplus_rebalance_overfilled (BplusTree *tree, BplusPath const *path) {

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

void bplus_rebalance_underfilled (BplusTree *tree, BplusPath const *path) {

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

void bplus_leaf_insert_at (BplusTree const *tree, BplusNode *node, const size_t index, 
    const BplusKey key, const BplusData value) {

    if (node && (index <= node->length)) {
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
            for (size_t i = index; i < index + length; ++i)
                bplus_node_at (node, i)->parent = node;
    }

}

void bplus_tree_insert (BplusTree *tree, const BplusKey key, const BplusData value) {

    if (tree) {
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

}

#pragma endregion

#pragma region BPLUS REMOVE

void bplus_node_remove_at (BplusTree const *tree, BplusNode *node, const size_t index, 
    const size_t length) {

    if (tree && node && (index < node->length) && (index + length <= node->length)) 
        bplus_node_move_and_resize_data (tree, node, index, index + length);

}

BplusData bplus_tree_remove_first (BplusTree *tree) {

    if (tree) {
        BplusPath path = { .leaf = (BplusNode *) tree->first };
        BplusNode *node = (BplusNode *) path.leaf;
        const size_t index = path.index[0];

        BplusData value = bplus_value_at (node, 0);
        bplus_node_remove_at (tree, node, index, 1);

        if (index == 0) bplus_rebalance_propagate (tree, &path);

        if (bplus_node_underfilled (node)) bplus_rebalance_underfilled (tree, &path);

        return value;
    }

    else return NULL;    

}

BplusData bplus_tree_remove (BplusTree *tree, const BplusKey key) {

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

// inits a new bplus tree
BplusTree *bplus_tree_new (void) {

    BplusTree *tree = (BplusTree *) malloc (sizeof (BplusTree));

    tree->first = bplus_leaf_new (tree);
    tree->last = tree->first;
    tree->root = (BplusNode *) tree->first;

    tree->height = 1;

    tree->node_count = 1;
    tree->leaf_count = 0;

    return tree;

}

void bplus_foreach_node_destroy (BplusTree *tree, BplusNode *node, void *arg) {

    if (tree && node)
        bplus_node_destroy (tree, node);

}

// TODO: add mor debugging information
void bplus_tree_destroy (BplusTree *tree) {

    if (tree) {
        bplus_foreach_node_in_tree (tree, &bplus_foreach_node_destroy, NULL);

        free (tree);

        #ifdef BPLUS_DEBUG
            fprintf (stdout, "Tree was successfully destroyed!\n");
        #endif
    }

}

// TODO: destroy all the items in the tree -> do we pass here our destroy function?
void bplus_tree_destroy_each (BplusTree *tree, BplusForeachItemFunc *foreach, 
    void *arg) {

    if (tree) {
        bplus_foreach_item_in_tree (tree, foreach, arg);
        bplus_tree_destroy (tree);
    }

}

BplusData bplus_tree_get_node_data (BplusTree *tree, const BplusKey key) {

    if (tree) {
        BplusPath path;
        bplus_tree_get_path_to_key (tree, key, &path);

        const size_t index = path.index[0];
        BplusNode const *node = path.leaf;
        BplusData value = NULL;

        if ((index < node->length) && bplus_key_eq (tree, bplus_key_at (node, index), key))
            value = bplus_value_at (node, index);

        return value;
    }

    return NULL;

}

BplusData bplus_tree_get_first_data (BplusTree *tree) {

    if (tree) {
        if (tree->first->node.length == 0) return NULL;
        else return tree->first->node.items[0].value;
    }

    return NULL;

}

BplusData bplus_tree_get_nth_data (BplusTree *tree, size_t pos) {

    if (tree && pos > 0) {
        BplusLeaf *leaf = tree->first;
        if (leaf->node.length == 0) return NULL;

        while (leaf != NULL && pos >= leaf->node.length) {
            pos -= leaf->node.length;
            leaf = leaf->right;
        }

        if (leaf) return leaf->node.items[pos].value;

        return NULL;
    }

    return NULL;

}

#pragma endregion

#pragma region DEBUG

void bplus_tree_print_stats (BplusTree const *tree) {

    if (tree) {
        fprintf (stdout, "\n--> Bplus tree stats <--\n");
        fprintf (stdout, "Height: %lu\n", tree->height);
        fprintf (stdout, "Node count: %lu\n", tree->node_count);
        fprintf (stdout, "Leaf count: %lu\n", tree->leaf_count);
    }

}

#pragma endregion