#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef SYMTABLE_H
#define SYMTABLE_H

#define STACK_MAX_SIZE 1000

//binary tree for functions and variables
typedef enum node_data_type {
    FUNCTION,
    VARIABLE_LET,
    VARIABLE_VAR
}bst_node_data_type;

typedef enum variable_type {
    String,
    Int,
    Double,
    String_nil,
    Int_nil,
    Double_nil
}variable_type;

typedef struct bst_node {
  char *key;              
  void *data;
  bst_node_data_type node_data_type;
  variable_type variable_type;            
  struct bst_node *left_child;  
  struct bst_node *right_child; 
} bst_node;

void bst_init(bst_node **tree);
bst_node *bst_search(bst_node *tree, char *key);
void bst_insert(bst_node **tree, char *key, bst_node_data_type data_type);
void bst_dispose(bst_node **tree);

//stack for scope

typedef struct stack {
    bst_node *stack_array[STACK_MAX_SIZE];
    int top;
} scope_stack;

scope_stack *scope_stack_init();
void scope_stack_push(scope_stack *stack);
void scope_stack_pop(scope_stack *stack);
bst_node *current_scope(scope_stack *stack);
void stack_dispose(scope_stack **stack);

//symtable inserting

typedef struct sym_t_variable {
    char *data;
}sym_t_variable;

typedef struct sym_t_function {

}sym_t_function;

void insert_variable_data(bst_node **tree, char *data);
void insert_variable_type(bst_node **tree, char *data);


void insert_function(bst_node **tree, char *key, sym_t_function *data, bst_node_data_type data_type);

#endif