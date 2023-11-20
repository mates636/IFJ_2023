#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "error_code.h"
#include "scanner.h"

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
    Not_specified,
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
bst_node *search_variable_in_all_scopes();
void bst_insert(bst_node **tree, char *key, bst_node_data_type data_type);
void bst_dispose(bst_node **tree);


//stack for scope
typedef struct stack {
    bst_node *stack_array[STACK_MAX_SIZE];
    int top;
} scope_stack;

scope_stack *scope_stack_init();
void scope_stack_push(scope_stack *stack);
error_t scope_stack_pop(scope_stack *stack);
bst_node *current_scope(scope_stack *stack);
void stack_dispose(scope_stack **stack);


//symtable inserting
typedef struct sym_t_variable {
    char *data;
}sym_t_variable;

typedef struct sym_t_param{
    char *param_name;
    char *param_id;
    variable_type param_type;
}sym_t_param;


typedef struct sym_t_function {
    char *id;
    variable_type return_type;
    sym_t_param *params;
    int num_params;
}sym_t_function;


void insert_variable_data(bst_node *tree, char *data);
void insert_variable_type(bst_node *tree, char *data);
void insert_function(bst_node **tree, char *key, sym_t_function *data);
variable_type find_variable_type(char *data);

//stack for paranthesis and brackets
typedef struct par_stack {
    char par_stack_array[STACK_MAX_SIZE];
    int top;
}par_stack;

par_stack *par_stack_init();
void par_stack_push(par_stack *par_stack, char c);
void par_stack_pop(par_stack *par_stack);
bool par_stack_is_empty(par_stack *par_stack);
void par_stack_dispose(par_stack **par_stack);

typedef struct expression_s{
    token_t *token_array[STACK_MAX_SIZE];
    int top;
}expression_s;

expression_s *expression_stack_init();
void expression_stack_push(expression_s *expression_stack, token_t *token);
token_t *expression_stack_pop(expression_s *expression_stack);
bool expression_stack_is_empty(expression_s *expression_stack);
void expression_stack_dispose(expression_s **expression_stack);


#endif