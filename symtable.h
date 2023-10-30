#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define STACK_MAX_SIZE 1000

//binary tree for functions and variables
typedef enum node_data_type {
    FUNCTION,
    VARIABLE
}bst_node_data_type;

typedef struct bst_node {
  char *key;              
  char *data;
  bst_node_data_type node_data_type;            
  struct bst_node *left_child;  
  struct bst_node *right_child; 
} bst_node;

void bst_init(bst_node **tree);
bst_node *bst_search(bst_node *tree, char *key);
void bst_insert(bst_node **tree, char *key, char *data);
void bst_dispose(bst_node **tree);

//stack for scope 
typedef struct stack_elem {
    bst_node *root;
} stack_elem;

typedef struct stack {
    bst_node *stack_array[STACK_MAX_SIZE];
    int top;
} scope_stack;

scope_stack *scope_stack_init();
void scope_stack_push(scope_stack *stack);
void scope_stack_pop(scope_stack *stack);
bst_node *scope_stack_top(scope_stack *stack);
void stack_dispose(scope_stack **stack);