#include "symtable.h"

#ifndef SYMTABLE_C
#define SYMTABLE_C


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

bst_node *search_variable_in_all_scopes(scope_stack *stack, char *key){
    bst_node *search;
    bst_node *my_node;
    int i = stack->top;

    while(i != -1){
        search = stack->stack_array[i];
        my_node = bst_search(search, key);
        if(my_node != NULL){
            return my_node;
        }
        i--;
    }
    return NULL;
}

void bst_insert(bst_node **tree, char *key, bst_node_data_type data_type){
    bst_node *parent_node = NULL;
    bst_node *tmp = (*tree);

    if((*tree) == NULL){
        bst_node *new_node = (bst_node*)malloc(sizeof(struct bst_node));
        new_node->key = key;
        new_node->data = NULL;
        new_node->variable_type = Not_specified;
        new_node->node_data_type = data_type;
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
            new_node->data = NULL;
            new_node->variable_type = Not_specified;
            new_node->node_data_type = data_type;
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
            new_node->data = NULL;
            new_node->variable_type = Not_specified;
            new_node->node_data_type = data_type;
            new_node->left_child= NULL;
            new_node->right_child = NULL;
            parent_node->right_child = new_node;
            return;
        }
        }else{
            return;
        }
  }
}

void bst_dispose(bst_node **tree){
    if((*tree) == NULL){
        return;
    }else{
        if ((*tree)->data != NULL) {
            free(((sym_t_variable *)(*tree)->data)->data);
            free((*tree)->data);
        }
        bst_dispose(&(*tree)->left_child);
        bst_dispose(&(*tree)->right_child);
        free(*tree);
        *tree = NULL;
    }


}


//stack functions
scope_stack *scope_stack_init(){
    scope_stack *stack = (scope_stack*)malloc(sizeof(scope_stack));
    stack->top = 0;

    bst_node *global_frame;
    bst_init(&global_frame);
    stack->stack_array[0] = global_frame;
    return stack; 
}

void scope_stack_push(scope_stack *stack){
    stack->top = stack->top + 1;

    bst_node *local_frame;
    bst_init(&local_frame);
    stack->stack_array[stack->top] = local_frame;
}

error_t scope_stack_pop(scope_stack *stack){
    if(stack->top == 0){
        return INTERN_ERROR;
    }

    bst_node *root = stack->stack_array[stack->top];
    bst_dispose(&root);
    stack->top = stack->top - 1;

    return SUCCESS;
}

bst_node *current_scope(scope_stack *stack){
    return stack->stack_array[stack->top];
}


void stack_dispose(scope_stack **stack){
    free(*stack);
}


//symtable inserting 
void insert_variable_data(bst_node *tree, char *data){
    int len = strlen(data);
    char *d = (char*)malloc((len + 1) * sizeof(char));
    strcpy(d, data);

    sym_t_variable *variable = (sym_t_variable*)malloc(sizeof(struct sym_t_variable));
    variable->data = d;
    
    tree->data = (sym_t_variable *)variable;
}

void insert_variable_type(bst_node *tree, char *data){
    if(strcmp(data, "Double") == 0){
        tree->variable_type = Double;
    }else if(strcmp(data, "Int") == 0){
        tree->variable_type = Int;
    }else if(strcmp(data, "String") == 0){
        tree->variable_type = String;
    }else if(strcmp(data, "String?") == 0){
        tree->variable_type = String_nil;
        insert_variable_data(&(*tree), "nil");
    }else if(strcmp(data, "Int?") == 0){
        tree->variable_type = Int_nil;
        insert_variable_data(&(*tree), "nil");
    }else if(strcmp(data, "Double?") == 0){
        tree->variable_type = Double_nil;
        insert_variable_data(&(*tree), "nil"); 
    }    
}

variable_type find_variable_type(char *data){
    if(strcmp(data, "Double") == 0){
        return  Double;
    }else if(strcmp(data, "Int") == 0){
        return  Int;
    }else if(strcmp(data, "String") == 0){
        return String;
    }else if(strcmp(data, "String?") == 0){
        return String_nil;
    }else if(strcmp(data, "Int?") == 0){
        return Int_nil;
    }else if(strcmp(data, "Double?") == 0){
        return Double_nil;
    }else{
        return Not_specified;
    }
}

void insert_function(bst_node **tree, char *key, sym_t_function *data){
    bst_insert(tree, key, FUNCTION);
    (*tree)->data = data;
}


//stack for paranthesis and brackets
par_stack *par_stack_init(){
    par_stack *stack = (par_stack*)malloc(sizeof(par_stack));
    stack->top = -1;
    return stack;
}

void par_stack_push(par_stack *par_stack, char c){
    par_stack->top = par_stack->top + 1;
    par_stack->par_stack_array[par_stack->top] = c;
}

error_t par_stack_pop(par_stack *par_stack){
    if(par_stack->top == -1){
        return SYNTAX_ERROR;
    }
    par_stack->top = par_stack->top - 1;
    return SUCCESS;
}
bool par_stack_is_empty(par_stack *par_stack){
    if(par_stack->top == 0){
        return true;
    }else{
        return false;
    }
}
void par_stack_dispose(par_stack **par_stack){
    (*par_stack)->top = 0;
    free((*par_stack));
}


expression_s *expression_stack_init(){
    expression_s *stack = (expression_s*)malloc(sizeof(expression_s));
    stack->top = -1;
    return stack;
}
void expression_stack_push(expression_s *expression_stack, token_t *token){
    token_t *t;
    size_t len;
    char *d;
    if(token->data == NULL){
        len = 1;
        d = (char *)malloc(sizeof(char) * 2);
        d[0] = ';';
        d[1] = '\0';
    }else{
        len = strlen(token->data) + 1;
        d = token->data;
    }    
    t = init_token_data(token->type, d, len);
    expression_stack->top = expression_stack->top + 1;
    expression_stack->token_array[expression_stack->top] = t;  
}

token_t *expression_stack_pop(expression_s *expression_stack){
    token_t *t;
    t = expression_stack->token_array[expression_stack->top];
    expression_stack->top = expression_stack->top - 1;
    return t;
}
bool expression_stack_is_empty(expression_s *expression_stack){
    if(expression_stack->top == 0){
        return true;
    }else{
        return false;
    }
}
void expression_stack_dispose(expression_s **expression_stack){
    free(*expression_stack);
}





#endif