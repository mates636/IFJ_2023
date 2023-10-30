#include "symtable.h"
//BST functions
void bst_init(bst_node **tree){
    (*tree) = NULL;
}

bst_node *bst_search(bst_node *tree, char *key){
    while(tree != NULL){
      if(strcmp(key, tree->key) < 0){
        tree = tree->left_child;
      }else if(strcmp(key, tree->key) > 0){
        tree = tree->right_child;
      }else{
        return tree;
      }  
    }
    return NULL;
}

void bst_insert(bst_node **tree, char *key, char *data){
    bst_node *parent_node = NULL;
    bst_node *tmp = (*tree);

    if((*tree) == NULL){
        bst_node *new_node = (bst_node*)malloc(sizeof(struct bst_node));
        new_node->key = key;
        new_node->data = data;
        new_node->left_child = NULL;
        new_node->right_child = NULL;
        (*tree) = new_node;
        return;
    }

    while (tmp != NULL) {
        parent_node = tmp;
        
        if(strcmp(key, tmp->key) < 0){
            tmp = tmp->left_child;
        if(tmp == NULL){
            bst_node *new_node = malloc(sizeof(struct bst_node));
            new_node->key = key;
            new_node->data = data;
            new_node->left_child = NULL;
            new_node->right_child = NULL;
            parent_node->left_child = new_node;
            return;
        }
        }else if(strcmp(key, tmp->key) > 0){
        tmp = tmp->right_child;
        if(tmp == NULL){
            bst_node *new_node = malloc(sizeof(struct bst_node));
            new_node->key = key;
            new_node->data = data;
            new_node->left_child= NULL;
            new_node->right_child = NULL;
            parent_node->right_child = new_node;
            return;
        }
        }else{
            tmp->data = data;
            return;
        }
  }
}

void bst_dispose(bst_node **tree){
    if((*tree) == NULL){
        return;
    }else{
        bst_dispose(&(*tree)->left_child);
        bst_dispose(&(*tree)->right_child);
        free(*tree);
        bst_init(&(*tree));
    }


}

//stack functions
scope_stack *scope_stack_init(){
    scope_stack *stack = (scope_stack*)malloc(sizeof(scope_stack));
    stack->top = 0;

    bst_node *global_frame;
    bst_init(&global_frame);
    stack->stack_array[0] = global_frame; 
}

void scope_stack_push(scope_stack *stack){
    stack->top = stack->top + 1;

    bst_node *local_frame;
    bst_init(&local_frame);
    stack->stack_array[stack->top] = local_frame;
}

void scope_stack_pop(scope_stack *stack){
    if(stack->top == 0){
        printf("cant pop global frame\n");
        return;
    }

    bst_node *root = stack->stack_array[stack->top];
    bst_dispose(&root);
    stack->top = stack->top - 1;
}

bst_node *scope_stack_top(scope_stack *stack){
    return stack->stack_array[stack->top];
}

void stack_dispose(scope_stack **stack){
    free(*stack);
}
