#include "rbtree.h"


ngx_rbtree_t test_tree;
ngx_rbtree_node_t test_sentinel;




int main(int argc, char *argv[])
{
    int ret = 0;

    ngx_rbtree_init(&test_tree, &test_sentinel, ngx_rbtree_insert_value);

    ngx_rbtree_node_t *node1 = calloc(1, sizeof(ngx_rbtree_node_t));
    node1->key = 1;

    ngx_rbtree_node_t *node2 = calloc(1, sizeof(ngx_rbtree_node_t));
    node2->key = 2;

    ngx_rbtree_node_t *node3 = calloc(1, sizeof(ngx_rbtree_node_t));
    node3->key = 3;

    ngx_rbtree_node_t *node4 = calloc(1, sizeof(ngx_rbtree_node_t));
    node4->key = 4;

    ngx_rbtree_insert(&test_tree, node1);
    ngx_rbtree_insert(&test_tree, node2);
    ngx_rbtree_insert(&test_tree, node3);
    ngx_rbtree_insert(&test_tree, node4);

    ngx_rbtree_node_t  *node;

    node = ngx_rbtree_min(test_tree.root, test_tree.sentinel);
    printf("node = %ld\n", node->key);

    node = ngx_rbtree_next(&test_tree, node);
    printf("node = %ld\n", node->key);

    node = ngx_rbtree_next(&test_tree, node);
    printf("node = %ld\n", node->key);

    node = ngx_rbtree_next(&test_tree, node);
    printf("node = %ld\n", node->key);

    node = ngx_rbtree_next(&test_tree, node);
    if (node == NULL) {
        printf("node == NULL\n");
    } else 
        printf("node = %ld\n", node->key);

    return ret;
}