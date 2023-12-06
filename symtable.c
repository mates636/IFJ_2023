/******************************************************************************
 *                                  IFJ23
 *                                symtable.c
 *
 *                  Authors: Mikuláš Uřídil(xuridi01)
 *           Purpose: Source file with implementation of symtable
 *
 *                      Last change: 6.12.2023
 *****************************************************************************/
#include "symtable.h"

#ifndef SYMTABLE_C
#define SYMTABLE_C

///////////////////////////////////// SYMTABLE functions
void bst_init(bst_node **tree)
{
    (*tree) = NULL;
    bst_insert(tree, "~root", FUNCTION);
}

void inorder_traversal(bst_node *root) {

    if (root != NULL) {
        inorder_traversal(root->left_child);
        inorder_traversal(root->right_child);
    }
    
}

bst_node *bst_search(bst_node *tree, char *key)
{
    
    while (tree != NULL)
    {
        if (strcmp(key, tree->key) < 0)
        {
            tree = tree->left_child;
        }
        else if (strcmp(key, tree->key) > 0)
        {
            tree = tree->right_child;
        }
        else
        {
            return tree;
        }
    }
    return NULL;
}

bst_node *search_variable_in_all_scopes(scope_stack *stack, char *key)
{
    bst_node *root;
    bst_node *my_node;
    bst_node *tmp = NULL;
    int i = stack->top;

    while (i != -1)
    {
        root = stack->stack_array[i];
        my_node = bst_search(root, key);

        if (my_node != NULL)
        {

            if(my_node->data == NULL){
                tmp = my_node;
                i--;
                continue;
            }
            return my_node;
        }
        i--;
    }

    if(tmp != NULL){

        return tmp;
    }else{
        return NULL;
    }
}


////////tree inserting and balancing functions
int max_height(int tree_node_height1, int tree_node_height2) {
    if(tree_node_height1 > tree_node_height2){
        return tree_node_height1;
    }else{
        return tree_node_height2;
    }
}

int height(bst_node *tree_node) {
    if(tree_node == NULL){
        return 0;
    }else{
        return tree_node->height;
    }
}

int get_height_difference(bst_node *tree_node) {
    if(tree_node == NULL){
        return 0;
    }else{
        return height(tree_node->left_child) - height(tree_node->right_child);
    }
}


//rotations for tree balance
bst_node *rotate_tree_right(bst_node *tree_node) {
    bst_node *new_parent = tree_node->left_child;
    bst_node *right_child_to_shift = new_parent->right_child;

    new_parent->right_child = tree_node;
    tree_node->left_child = right_child_to_shift;

    tree_node->height = max_height(height(tree_node->left_child), height(tree_node->right_child)) + 1;
    new_parent->height = max_height(height(new_parent->left_child), height(new_parent->right_child)) + 1;

    return new_parent;
}

bst_node *rotate_tree_left(bst_node *tree_node) {
    bst_node *new_parent = tree_node->right_child;
    bst_node *left_child_to_shift = new_parent->left_child;

    new_parent->left_child = tree_node;
    tree_node->right_child = left_child_to_shift;

    tree_node->height = max_height(height(tree_node->left_child), height(tree_node->right_child)) + 1;
    new_parent->height = max_height(height(new_parent->left_child), height(new_parent->right_child)) + 1;

    return new_parent;
}

//new node insert
bst_node *new_node(char *key, bst_node_data_type data_type) {
    bst_node *new_node = malloc(sizeof(struct bst_node));
    new_node->key = string_copy(key);
    new_node->data = NULL;
    new_node->variable_type = Not_specified;
    new_node->node_data_type = data_type;
    new_node->left_child = NULL;
    new_node->right_child = NULL;
    new_node->height = 1;
    return new_node;
}

//inserting nodes
void bst_insert(bst_node **tree, char *key, bst_node_data_type data_type) {
    if ((*tree) == NULL) {

        *tree = new_node(key, data_type);
        return;
    }
    //going left
    if (strcmp(key, (*tree)->key) < 0) {
        bst_insert(&((*tree)->left_child), key, data_type);
    
    //going right
    } else if (strcmp(key, (*tree)->key) > 0) {
        bst_insert(&((*tree)->right_child), key, data_type);
    
    } else {
        return;
    }
    
    (*tree)->height = max_height(height((*tree)->left_child), height((*tree)->right_child)) + 1;

    //need balance the tree
    
    int height_diff = get_height_difference(*tree);

    if (height_diff > 1) {
        if (strcmp(key, (*tree)->left_child->key) < 0) {
            (*tree) = rotate_tree_right(*tree);
        } else {
            //need to put keys right
            (*tree)->left_child = rotate_tree_left((*tree)->left_child);
            (*tree) = rotate_tree_right(*tree);
        }
    }

    if (height_diff < -1) {
        if (strcmp(key, (*tree)->right_child->key) > 0) {
            (*tree) = rotate_tree_left(*tree);
        } else {
            //need to put keys right
            (*tree)->right_child = rotate_tree_right((*tree)->right_child);
            (*tree) = rotate_tree_left(*tree);
        }
    }
}
////////////////////////////////////////////////////////////


void bst_print(bst_node *tree)
{
    if (tree == NULL)
    {
        return;
    }
    else
    {
        bst_print(tree->left_child);
        bst_print(tree->right_child);
    }
}

void bst_dispose(bst_node **tree)
{
    if ((*tree) == NULL)
    {
        return;
    }
    else
    {
        if ((*tree)->data != NULL)
        {
            if ((*tree)->node_data_type == FUNCTION)
            {
                sym_t_function *fun = (*tree)->data;

                free(fun->id);
                for (int i = 0; i < fun->num_params; i++)
                {
                    free(fun->params[i].param_name);
                    free(fun->params[i].param_id);
                }
                free(fun->params);
                free((*tree)->data);
            }
            else
            { // VARIABLE_VAR or VARIABLE_LET
                free(((sym_t_variable *)(*tree)->data)->data);
                free((*tree)->data);
            }
        }
        free((*tree)->key);
        bst_dispose(&(*tree)->left_child);
        bst_dispose(&(*tree)->right_child);
        free(*tree);
        *tree = NULL;
    }
}

char *string_copy(char *src)
{
    char *dst = malloc(strlen(src) + 1);
    strcpy(dst, src);
    return dst;
}


// stack functions - each element of stack represents one local scope tree, starting from global
scope_stack *scope_stack_init()
{
    scope_stack *stack = (scope_stack *)malloc(sizeof(scope_stack));
    stack->top = 0;

    bst_node *global_frame;
    bst_init(&global_frame);
    stack->stack_array[0] = global_frame;
    return stack;
}

void scope_stack_push(scope_stack *stack)
{
    stack->top = stack->top + 1;

    bst_node *local_frame;
    bst_init(&local_frame);

    stack->stack_array[stack->top] = local_frame;
}

error_t scope_stack_pop(scope_stack *stack)
{
    if (stack->top == 0)
    {
        return INTERN_ERROR;
    }

    bst_node *root = stack->stack_array[stack->top];
    bst_dispose(&root);
    stack->top = stack->top - 1;

    return SUCCESS;
}

bst_node *current_scope(scope_stack *stack)
{
    return stack->stack_array[stack->top];
}

void stack_dispose(scope_stack **stack)
{
    free(*stack);
}

// symtable inserting
void insert_variable_data(bst_node *tree, char *data)
{
    int len = strlen(data);
    char *d = (char *)malloc((len + 1) * sizeof(char));
    strcpy(d, data);

    sym_t_variable *variable = (sym_t_variable *)malloc(sizeof(struct sym_t_variable));
    variable->data = d;

    tree->data = (sym_t_variable *)variable;
}

void insert_variable_type(bst_node *tree, char *data)
{
    if (strcmp(data, "Double") == 0)
    {
        tree->variable_type = Double;
    }
    else if (strcmp(data, "Int") == 0)
    {
        tree->variable_type = Int;
    }
    else if (strcmp(data, "String") == 0)
    {
        tree->variable_type = String;
    }
    else if (strcmp(data, "String?") == 0)
    {
        tree->variable_type = String_nil;
    }
    else if (strcmp(data, "Int?") == 0)
    {
        tree->variable_type = Int_nil;
    }
    else if (strcmp(data, "Double?") == 0)
    {
        tree->variable_type = Double_nil;
    }
}

variable_type find_variable_type(char *data)
{
    if (strcmp(data, "Double") == 0)
    {
        return Double;
    }
    else if (strcmp(data, "Int") == 0)
    {
        return Int;
    }
    else if (strcmp(data, "String") == 0)
    {
        return String;
    }
    else if (strcmp(data, "String?") == 0)
    {
        return String_nil;
    }
    else if (strcmp(data, "Int?") == 0)
    {
        return Int_nil;
    }
    else if (strcmp(data, "Double?") == 0)
    {
        return Double_nil;
    }
    else
    {
        return Not_specified;
    }
}

void insert_function(bst_node **tree, char *key, sym_t_function *data, scope_stack *stack)
{
    bst_insert(tree, key, FUNCTION);
    stack->stack_array[stack->top] = (*tree);
    bst_node *fun_node = bst_search(*tree, key);
    fun_node->data = (sym_t_function *)data;
}
////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////// stack for paranthesis and brackets///////////////////////////////
par_stack *par_stack_init()
{
    par_stack *stack = (par_stack *)malloc(sizeof(par_stack));
    stack->top = -1;
    return stack;
}

void par_stack_push(par_stack *par_stack, char c)
{
    par_stack->top = par_stack->top + 1;
    par_stack->par_stack_array[par_stack->top] = c;
}

error_t par_stack_pop(par_stack *par_stack)
{
    if (par_stack->top == -1)
    {
        return SYNTAX_ERROR;
    }
    par_stack->top = par_stack->top - 1;
    return SUCCESS;
}
bool par_stack_is_empty(par_stack *par_stack)
{
    if (par_stack->top == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
void par_stack_dispose(par_stack **par_stack)
{
    (*par_stack)->top = 0;
    free((*par_stack));
}
///////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////functions for precedence analyse///////////////////////////////////////
expression_s *expression_stack_init()
{
    expression_s *stack = (expression_s *)malloc(sizeof(expression_s));
    stack->top = -1;
    return stack;
}
void expression_stack_push(expression_s *expression_stack, token_t *token)
{
    token_t *t;
    size_t len;
    char *d;
    if (token->data == NULL)
    {
        len = 1;
        d = (char *)malloc(sizeof(char) * 2);
        d[0] = ';';
        d[1] = '\0';
    }
    else
    {
        len = strlen(token->data) + 1;
        d = token->data;
    }
    t = init_token_data(token->type, d, len);
    expression_stack->top = expression_stack->top + 1;
    expression_stack->token_array[expression_stack->top] = t;
}

token_t *expression_stack_pop(expression_s *expression_stack)
{
    token_t *t;
    t = expression_stack->token_array[expression_stack->top];
    expression_stack->top = expression_stack->top - 1;
    return t;
}
bool expression_stack_is_empty(expression_s *expression_stack)
{
    if (expression_stack->top == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
void expression_stack_dispose(expression_s **expression_stack)
{
    free(*expression_stack);
}
///////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////others//////////////////////////////////////////////////////////
char *variable_type_to_str(variable_type t)
{
    switch (t)
    {
    case Not_specified:
        return "Not_specified";
    case Nil:
        return "Nil";
    case String:
        return "String";
    case Int:
        return "Int";
    case Double:
        return "Double";
    case String_nil:
        return "String_nil";
    case Int_nil:
        return "Int_nil";
    case Double_nil:
        return "Double_nil";
    case Void:
        return "Void";
    default:
        return "??";
    }
}

#endif