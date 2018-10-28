#include <stdlib.h>

#include "binary_search_tree.h"


/* insertion and deletion */

struct bst_node * bst_insert(struct bst_node *root, void *key, void *data,
                             int(*comp)(void const *, void const *))
{
    struct bst_node *tmp = malloc(sizeof(struct bst_node));
    if (!tmp)
        return NULL;

    tmp->key = key;
    tmp->data = data;
    tmp->left = NULL;
    tmp->right = NULL;

    if (!root) {
        tmp->parent = NULL;
        return tmp;
    }

    struct bst_node *current, *parent;
    parent = NULL;
    current = root;

    while (current) {
        parent = current;
        if (comp(key, current->key) < 0)
            current = current->left;
        else
            current = current->right;
    }

    tmp->parent = parent;
    if (comp(key, parent->key) < 0)
        parent->left = tmp;
    else
        parent->right = tmp;

    return root;
}

void bst_free(struct bst_node *root, int free_keys, int free_data)
{
    if (!root)
        return;

    bst_free(root->left, free_keys, free_data);
    bst_free(root->right, free_keys, free_data);

    if (free_keys)
        free(root->key);

    if (free_data)
        free(root->data);

    free(root);
}


/* searching */

struct bst_node * bst_search(struct bst_node *node, void const *key,
                             int(*comp)(void const *, void const *))
{
    int tmp;

    while (node) {
        tmp = comp(key, node->key);

        if (tmp == 0)
            return node;

        if (tmp < 0)
            node = node->left;
        else
            node = node->right;
    }

    return NULL;
}

struct bst_node * bst_min(struct bst_node *node)
{
    if (!node)
        return NULL;

    while (node->left)
        node = node->left;

    return node;
}

struct bst_node * bst_max(struct bst_node *node)
{
    if (!node)
        return NULL;

    while (node->right)
        node = node->right;

    return node;
}

struct bst_node * bst_predecessor(struct bst_node *node)
{
    if (!node)
        return NULL;

    if (node->left)
        return bst_max(node->left);

    struct bst_node *parent = node->parent;
    while (parent && node == parent->left) {
        node = parent;
        parent = parent->parent;
    }

    return parent;
}

struct bst_node * bst_successor(struct bst_node *node)
{
    if (!node)
        return NULL;

    if (node->right)
        return bst_min(node->right);

    struct bst_node *parent = node->parent;
    while (parent && node == parent->right) {
        node = parent;
        parent = parent->parent;
    }

    return parent;
}


/* inorder iteration */

struct bst_iter {
    struct bst_node *last, *current;
};

struct bst_iter *bst_iter_create(struct bst_node *node)
{
    struct bst_iter *it = malloc(sizeof(struct bst_iter));

    it->last = NULL;
    it->current = node;

    return it;
}

void bst_iter_free(struct bst_iter *it)
{
    free(it);
}

int bst_iter_has_next(struct bst_iter const *it)
{
    return it->current != NULL;
}

struct bst_node * bst_iter_next(struct bst_iter *it)
{
    struct bst_node *next = NULL;

    while (!next) {
        if (!it->current)
            return NULL;

        if (it->last == it->current->parent) {
            it->last = it->current;

            if (it->current->left) {
                it->current = it->current->left;
            } else if (it->current->right) {
                next = it->current;
                it->current = it->current->right;
            } else {
                next = it->current;
                it->current = it->current->parent;
            }

        } else if (it->last == it->current->left) {
            next = it->current;

            it->last = it->current;

            if (it->current->right)
                it->current = it->current->right;
            else
                it->current = it->current->parent;

        } else if (it->last == it->current->right) {
            it->last = it->current;

            it->current = it->current->parent;
        }
    }

    return next;
}
