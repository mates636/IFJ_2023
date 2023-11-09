#include "parser.h"

#ifndef PARSER_C
#define PARSER_C

scope_stack *stack;

//token for decision if i got next expression or not
token_t *next_token = NULL;

void init_parser(){
    stack = scope_stack_init();
}

void free_parser(){
    stack_dispose(&stack);
}

error_t run_parser(scanner_t *scanner){
    token_t *token;
    error_t error;

    while(true){
        //checking if i didn't start next expression in expression before
        if(next_token == NULL){
            error = get_token(scanner, &token);

            //checking error from scanner
            if(error != SUCCESS){
                return error;
            }

            //end of file for compilation
            if(token->type == EOF_TYPE){
                destroy_token(token);
                break;
            }

            //start of checking which expression i got
            error = parser_analyse(scanner, token); 

            if(error != SUCCESS){
                printf("chyba %d \n", error);
                destroy_token(token);
                return error;
            }
            
            destroy_token(token);
        }else{

            //end of file for compilation
            if(next_token->type == EOF_TYPE){
                destroy_token(next_token);
                break;
            }

            //start of checking which expression i got
            error = parser_analyse(scanner, next_token); 

            if(error != SUCCESS){
                printf("chyba %d \n", error);
                destroy_token(next_token);
                return error;
            }
            
            destroy_token(next_token);
        }
    }
    return SUCCESS;
}

//decision for which expression we are going
error_t parser_analyse(scanner_t *scanner, token_t *token){
    switch (token->type)
    {
    case KEYWORD:
        if(token->data == "let"){
            return parser_variable(scanner, token);
        }else if(token->data == "var"){
            return parser_variable(scanner, token);
        /*}else if(token->data == "if"){
            return parser_if_statement();
        }else if(token->data == "func"){
            return parser_function();
        }else else if(token->data == "while"){
            return parser_while_statement();
        */}else{
            return SYNTAX_ERROR;
        }
    default:
        return SYNTAX_ERROR;
    }
}



error_t parser_assignment(token_t *token){
    if(token->type != ASSIGMENT){
        return SYNTAX_ERROR;
    }else{
        return SUCCESS;
    }
}

error_t parser_colon(token_t *token){
    if(token->type != COLON){
        return SYNTAX_ERROR;
    }else{
        return SUCCESS;
    }
}

error_t parser_data_type(token_t *token){
    if(token->type == KEYWORD){
        if(token->data == "String" || token->data == "Double" || token->data == "Int"){
            return SUCCESS;
        }//else if(){
            //TO DOOOOOO FUNKCE
        //}
        else{
            return SYNTAX_ERROR;
        }
    }else{
        return SYNTAX_ERROR;
    }
}

///////////////////////////NEW VARIABLE /////////////////////////////////////
error_t parser_variable(scanner_t *scanner, token_t *t){
    bool can_modify;
    if(t->data == "let"){
        can_modify = false;
    }else{
        can_modify = true;
    }

    token_t *token;
    error_t error = get_token(scanner, &token);
    if(error != SUCCESS){
            return error;
        }

    if(token->type == IDENTIFIER){
        return parser_variable_id_type_value(scanner, token, can_modify);
    }else{
        destroy_token(token);
        return SYNTAX_ERROR;
    }
    
}

error_t parser_variable_id_type_value(scanner_t *scanner, token_t *token, bool can_modify){ ///VALUE CAN BE FUNCTION - TO DOO
    bst_node_data_type node_data_type;
    if(can_modify){
        node_data_type = VARIABLE_VAR;
    }else{
        node_data_type = VARIABLE_LET;
    }

    //search for existing name of variable in this scope
    bst_node *identifier = bst_search(current_scope(stack), token->data);
    if(identifier == NULL){
        //we can insert new node with key of the variable
        bst_insert(&identifier, token->data, node_data_type);
    }else{
        return SEMANTIC_ERROR_UNDEF_FUN_OR_REDEF_VAR;
    }

    //getting next token
    token_t* t;
    error_t error = get_token(scanner, &t);
    if(error != SUCCESS){
        destroy_token(token);
        destroy_token(t);
        return error;
    }


    //next is colon
    if(parser_colon(token) == SUCCESS){
        error = get_token(scanner, &t);
        if(error != SUCCESS){
            destroy_token(token);
            destroy_token(t);
            return error;
        }

        //need data type of variable
        error = parser_data_type(t);
        if(error != SUCCESS){
            destroy_token(token);
            destroy_token(t);
            return error;
        }

        insert_variable_type(&identifier, t->data);

        //assignment or another expression
        error = get_token(scanner, &t);
        if(error != SUCCESS){
            destroy_token(token);
            destroy_token(t);
            return error;
        }

        if(parser_assignment(t) != SUCCESS){
            next_token = t;
            return SUCCESS;
        }

    }else{
        destroy_token(token);
        destroy_token(t);
        return SYNTAX_ERROR;
    }


    //next is assignment
    if(parser_assignment(t) == SUCCESS){
        error = get_token(scanner, &t);
        if(error != SUCCESS){
            destroy_token(token);
            destroy_token(t);
            return error;
        }

        insert_variable_data(&identifier, t->data);
        if(t->type == STRING){
            identifier->variable_type = String;
        }else if(t->type == INT){
            identifier->variable_type = Int;
        }else if(t->type == DOUBLE){
            identifier->variable_type = Double;
        }
        return SUCCESS;
    }else{
        next_token = t;
        return SUCCESS;
    }
}
///////////////////////////////////////////////////////////////////////////////

#endif