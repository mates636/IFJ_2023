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

    bst_insert(&(stack->stack_array[stack->top]), "ahoj", "32");
    printf("%s\n", stack->stack_array[stack->top]->key);
    bst_node *node = bst_search(stack->stack_array[stack->top], "ahoj");
    printf("%s\n", node->data);


    bst_insert(&(stack->stack_array[stack->top]), "ahoj", "32");
    bst_insert(&(stack->stack_array[stack->top]), "bus", "33");
    bst_insert(&(stack->stack_array[stack->top]), "cesta", "34");
    bst_insert(&(stack->stack_array[stack->top]), "honza", "35");

    node = bst_search(stack->stack_array[stack->top], "honza");
    printf("%s\n", node->data);
    
    if((node = bst_search(stack->stack_array[stack->top], "dum")) == NULL){
        printf("not found\n");
    }else{
        printf("%s\n", node->data);
    }


    scope_stack_push(stack);
    bst_insert(&(stack->stack_array[stack->top]), "1", "local1");
    bst_insert(&(stack->stack_array[stack->top]), "5", "local5");

    node = bst_search(stack->stack_array[stack->top], "1");
    printf("%s\n", node->data);

    node = bst_search(stack->stack_array[0], "cesta");
    printf("%s\n", node->data);

    scope_stack_pop(stack);
    if((node = bst_search(stack->stack_array[stack->top], "1")) == NULL){
        printf("not found\n");
    }else{
        printf("%s\n", node->data);
    }

    stack_dispose(&stack);
    if((node = bst_search(stack->stack_array[stack->top], "cesta")) == NULL){
        printf("disposed\n");
    }else{
        printf("%s\n", node->data);
    }


    return 0;
}