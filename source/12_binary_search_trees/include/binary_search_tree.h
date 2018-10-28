// TODO: prevent tree modification via return node pointers?

#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H


/* basic data structures */

struct bst_node {
    struct bst_node *left, *right, *parent;
    void *key;
    void *data;
};


/* insertion and deletion */

struct bst_node * bst_insert(struct bst_node *root, void *key, void *data,
                             int(*comp)(void const *, void const *));

struct bst_node * bst_delete(struct bst_node *root, struct bst_node *node,
                             int free_key, int free_data);

void bst_free(struct bst_node *root, int free_keys, int free_data);


/* searching */

struct bst_node * bst_search(struct bst_node *root, void const *key,
                             int(*comp)(void const *, void const *));

struct bst_node * bst_min(struct bst_node *root);
struct bst_node * bst_max(struct bst_node *root);
struct bst_node * bst_predecessor(struct bst_node *node);
struct bst_node * bst_successor(struct bst_node *node);


/* inorder iteration */

struct bst_iter;

struct bst_iter * bst_iter_create(struct bst_node *root);
void bst_iter_free(struct bst_iter *it);

int bst_iter_has_next(struct bst_iter const *it);
struct bst_node * bst_iter_next(struct bst_iter *it);

#endif
