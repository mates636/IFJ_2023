#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "symtable.h"

int main(){

    scope_stack *stack;
    stack = scope_stack_init();

    if(stack->stack_array[stack->top] == NULL){
        printf("good\n");
    }else{
        printf("bad\n");
    }

    bst_insert(&(stack->stack_array[stack->top]), "ahoj", VARIABLE_LET);
    printf("%s\n", stack->stack_array[stack->top]->key);

    bst_node *node = bst_search(stack->stack_array[stack->top], "ahoj");
    insert_variable_data(&node, "pepa");

    node = bst_search(stack->stack_array[stack->top], "ahoj");
    struct sym_t_variable *variable = (struct sym_t_variable *)(node->data);
    
    printf("%s\n", node->key);
    printf("%s\n", variable->data);
    
    if((node = bst_search(stack->stack_array[stack->top], "dum")) == NULL){
        printf("not found\n");
    }else{
        printf("%s\n", node->key);
    }


    scope_stack_push(stack);
    bst_insert(&(stack->stack_array[stack->top]), "1", VARIABLE_LET);
    bst_insert(&(stack->stack_array[stack->top]), "5", VARIABLE_LET);

    node = bst_search(stack->stack_array[stack->top], "1");
    printf("%s\n", node->key);



    scope_stack_pop(stack);
    if((node = bst_search(stack->stack_array[stack->top], "1")) == NULL){
        printf("not found\n");
    }else{
        printf("%s\n", node->key);
    }

    stack_dispose(&stack);
    if((node = bst_search(stack->stack_array[stack->top], "cesta")) == NULL){
        printf("disposed\n");
    }else{
        printf("%s\n", node->key);
    }


    return 0;
}