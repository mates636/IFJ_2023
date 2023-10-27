#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


//binary tree for functions and variables
typedef enum node_data_type {
    FUNCTION,
    VARIABLE
}node_data_type;

typedef struct bst_node_t {
  char *key;              
  void *value;
  node_data_type node_data_type;            
  struct bst_node *left;  
  struct bst_node *right; 
} bst_node_t;

void bst_init(bst_node_t **tree);
void bst_insert(bst_node_t **tree, char *key, int value);
bst_node_t bst_search(bst_node_t *tree, char *key, int *value);
void bst_delete(bst_node_t **tree, char *key);
void bst_dispose(bst_node_t **tree);

//stack for frames - starts with Global frame
