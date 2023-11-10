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

///////////////////////////HELP FUNCTIONS////////////////////////////////
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

error_t parser_variable_datatype(token_t *token){
    if(token->type == KEYWORD){
        if(token->data == "String" || token->data == "Double" || token->data == "Int"){
            return SUCCESS;
        }else{
            return SYNTAX_ERROR;
        }
    }else{
        return SYNTAX_ERROR;
    }
}

error_t can_be_variable_value(bst_node *tree_node, token_t *token){
    if( token->type == IDENTIFIER ||
        token->type == STRING ||
        token->type == INT ||
        token->type == DOUBLE || 
        token->type == LEFT_PAR ||
        token->type == FUNC_IDENTRIFIER ||
        token->type == NIL){

        switch(token->type){
            case IDENTIFIER:

            case LEFT_PAR:

            case FUNC_IDENTRIFIER:

            case STRING:
                if( tree_node->variable_type == String ||
                    tree_node->variable_type == String_nil ||
                    tree_node->variable_type == Not_specified){
                    
                    insert_variable_data(tree_node, token->data);
                    if(tree_node->variable_type == Not_specified){
                        tree_node->variable_type = String;
                    }
                    return SUCCESS;
                }else{
                    return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                }
            case INT:
                if( tree_node->variable_type == Int ||
                    tree_node->variable_type == Int_nil ||
                    tree_node->variable_type == Not_specified){
                    
                    insert_variable_data(tree_node, token->data);
                    if(tree_node->variable_type == Not_specified){
                        tree_node->variable_type = Int;
                    }
                    return SUCCESS;
                }else{
                    return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                }
            case DOUBLE:
                if( tree_node->variable_type == Double ||
                    tree_node->variable_type == Double_nil ||
                    tree_node->variable_type == Not_specified){
                    
                    insert_variable_data(tree_node, token->data);
                    if(tree_node->variable_type == Not_specified){
                        tree_node->variable_type = Double;
                    }
                    return SUCCESS;
                }else{
                    return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                }
            case NIL:
                if(tree_node->variable_type == Not_specified){
                    return SEMANTIC_ERROR_TYPE_CANNOT_INFERRED;
                }
                else if(tree_node->variable_type == Int_nil ||
                    tree_node->variable_type == Double_nil ||
                    tree_node->variable_type == String_nil){
                    
                    insert_variable_data(tree_node, token->data);
                    return SUCCESS;
                }else{
                    return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                }
        }
    }else{
        return SYNTAX_ERROR;
    }
}
/////////////////////////////////////////////////////////////////////////////

///////////////////////////NEW VARIABLE /////////////////////////////////////
error_t parser_variable(scanner_t *scanner, token_t *token){
    bool can_modify;
    if(token->data == "let"){
        can_modify = false;
    }else{
        can_modify = true;
    }

    error_t error = get_token(scanner, &token);
    if(error != SUCCESS){
            return error;
        }

    if(token->type == IDENTIFIER){
        return parser_variable_identifier(scanner, token, can_modify);
    }else{
        return SYNTAX_ERROR;
    }
    
}

error_t parser_variable_identifier(scanner_t *scanner, token_t *token, bool can_modify){ ///VALUE CAN BE FUNCTION - TO DOO
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

    parser_variable_type_and_data(scanner, token, identifier);
}

error_t parser_variable_type_and_data(scanner_t *scanner, token_t *token, bst_node *tree_node){
    error_t error = get_token(scanner, &token);
    if(error != SUCCESS){
        return error;
    }

    bool valid_expression = false;

    //next is colon
    if(parser_colon(token) == SUCCESS){
        valid_expression = true;
        error = get_token(scanner, &token);
        if(error != SUCCESS){
            return error;
        }

        //need data type of variable
        error = parser_variable_datatype(token);
        if(error != SUCCESS){
            return error;
        }

        //inserting type of variable
        insert_variable_type(tree_node, token->data);

        //assignment or another expression
        error = get_token(scanner, &token);
        if(error != SUCCESS){
            return error;
        }

        if(parser_assignment(token) != SUCCESS){
            next_token = token;
            return SUCCESS;
        }
    }

    //next is assignment
    if(parser_assignment(token) == SUCCESS){
        valid_expression = true;
        error = get_token(scanner, &token);
        if(error != SUCCESS){
            return error;
        }

        //checking if we have correct right value
        error = can_be_variable_value(tree_node, token);
        if(error != SUCCESS){
            return error;
        }else{
            return SUCCESS;
        }
    }

    //control if i got at least type or value
    if(valid_expression == true){
        return SUCCESS;
    }else{
        return SYNTAX_ERROR;
    }
}
///////////////////////////////////////////////////////////////////////////////

#endif