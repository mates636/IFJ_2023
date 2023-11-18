#include "parser.h"

#ifndef PARSER_C
#define PARSER_C

scope_stack *stack;//symtable
par_stack *p_stack;

//token for decision if i got next expression or not
token_t *next_token = NULL;

void init_parser(){
    stack = scope_stack_init();
    p_stack = par_stack_init();
}

void free_parser(){
    stack_dispose(&stack);
    par_stack_dispose(&p_stack);
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
                destroy_token(token);
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
    printf("succes\n");
    return SUCCESS;
}

//decision for which expression we are going
error_t parser_analyse(scanner_t *scanner, token_t *token){
    switch (token->type)
    {
        case KEYWORD:
            if(strcmp(token->data, "let") == 0){
                return parser_variable(scanner, token);
            }else if(strcmp(token->data, "var") == 0){
                return parser_variable(scanner, token);
            /*}else if(token->){
                return parser_expression(scanner, token);
            }else if(token->data == "func"){
                return parser_function();
            }else else if(token->data == "while"){
                return parser_while_statement();
            */}else{
                return SYNTAX_ERROR;
            }
        case LEFT_PAR:
        par_stack_push(p_stack, '(');
            return parser_expression(scanner, token, NULL);
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

error_t can_be_variable_value(scanner_t *scanner, bst_node *tree_node, token_t *token){
    if( token->type == IDENTIFIER ||
        token->type == STRING ||
        token->type == INT ||
        token->type == DOUBLE || 
        token->type == LEFT_PAR ||
        token->type == FUNC_IDENTRIFIER ||
        token->type == NIL){
        
        bst_node *right_variable;

        switch(token->type){
            case IDENTIFIER:
                right_variable = search_variable_in_all_scopes(stack, token->data);
                if(right_variable == NULL){
                    return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
                }else{
                    if(tree_node->variable_type == Not_specified){
                        tree_node->variable_type = right_variable->variable_type;
                    }
                    if(right_variable->variable_type != tree_node->variable_type){
                        return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                    }else{
                        sym_t_variable *variable = (sym_t_variable*)right_variable->data;
                        insert_variable_data(tree_node, variable->data);
                        return SUCCESS;
                    }
                }
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
        error = can_be_variable_value(scanner, tree_node, token);
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

///////////////////////////HELP FUNCTIONS - EXPRESSION////////////////////////
error_t parser_expression_type_control_first_value(token_t *token, variable_type *type_control){
    bst_node *id; 
    switch(token->type){
            case IDENTIFIER:
                id = search_variable_in_all_scopes(stack, token->data);
                if(id == NULL){
                    return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
                }else{
                    if(id->data == NULL){
                        return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
                    }
                }
                (*type_control) = id->variable_type;
                if((*type_control) == String_nil || (*type_control) == Int_nil || (*type_control) == Double_nil){
                   return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL; 
                }
                return SUCCESS;
            case NIL:
                return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL; 
            case STRING:
                (*type_control) = String;
                return SUCCESS; 
            case INT:
                (*type_control) = Int;
                return SUCCESS; 
            case DOUBLE:
                (*type_control) = Double;
                return SUCCESS; 
        }
}

//expression type control for arithmetic and string operations
error_t parser_expression_type_control_arithmetic_strings(token_t *token, variable_type *type_control){
    bst_node *id; 
    
    //first value in expression
    if((*type_control) == Not_specified){
        return parser_expression_type_control_first_value(token, type_control);
    //other values in expression - checking types validity
    }else{
        token_t *tmp = token;
        if(tmp->type == IDENTIFIER){
            id = search_variable_in_all_scopes(stack, token->data);
            if(id == NULL){
                    return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
                }
            if(id->variable_type == String){
                tmp->type = STRING;
            }else if(id->variable_type == Double){
                tmp->type = DOUBLE;
            }else if(id->variable_type == Int){
                tmp->type = INT;
            }else{
                return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
            }
        }

        switch(tmp->type){
            case STRING:
                if((*type_control) != String){
                    return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                }
                break;
            case DOUBLE:
                if((*type_control) == Double || (*type_control) == Int){
                    (*type_control) = Double;
                }else{
                    return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                }
                break;
            case INT:
                if((*type_control) == String){
                    return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                }
                break;
        }
        return SUCCESS;
    }
}

error_t parser_expression_type_control_rel_operators(token_t *token, variable_type *type_control, token_type_t type_token){
    bst_node *id;
    
    //first value in expression
    if((*type_control) == Not_specified){
        return parser_expression_type_control_first_value(token, type_control);
    //other values in expression - checking types validity
    }else{
        if(token->type == IDENTIFIER){
            id = search_variable_in_all_scopes(stack, token->data);
            if(id == NULL){
                    return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
                }else{
                    if(id->data == NULL){
                        return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
                    }
                }
            if(id->variable_type != (*type_control)){
                return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
            }else{
                return SUCCESS;
            }
        }
        if(type_token == EQUALS || type_token == NOT_EQUALS){
            switch(token->type){
                case STRING:
                    if((*type_control) != String){
                        return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                    }
                    break;
                case DOUBLE:
                    if((*type_control) == Double || (*type_control) == Int){
                        (*type_control) = Double;
                    }else{
                        return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                    }
                    break;
                case INT:
                    if((*type_control) == String){
                        return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                    }
                    break;
            }
            return SUCCESS;
        }else{
            switch(token->type){
                case STRING:
                    if((*type_control) != String){
                        return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                    }
                    break;
                case DOUBLE:
                    if((*type_control) != Double){
                        return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                    }
                    break;
                case INT:
                    if((*type_control) != Int){
                        return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                    }
                    break;
            }   
            return SUCCESS;
        }
    }
}

error_t parser_expression_nil_convert(token_t **left_token, token_t *right_token){
     bst_node *variable;

     if((*left_token)->type == IDENTIFIER){
        //control of both sides of operator
        variable_type right_side;
        variable_type left_side;

        //getting type of left side
        variable = search_variable_in_all_scopes(stack, (*left_token)->data);
        left_side = variable->variable_type;
        
        //getting type of right side
        if(right_token->type = IDENTIFIER){
            variable = search_variable_in_all_scopes(stack, right_token->data);
            if(variable->variable_type == String_nil || variable->variable_type == Int_nil || variable->variable_type == Double_nil){
                return SEMANTIC_ERROR_OTHERS;
            right_side = variable->variable_type;
            }else{
                switch(right_token->type){
                    case INT:
                        right_side = Int;
                    case DOUBLE:
                        right_side = Double;
                    case STRING:
                        right_side = String;
                    default:
                        return SYNTAX_ERROR;
                }
            }
        
            //type compatibilty
            if(left_side != right_side){
                return SEMANTIC_ERROR_OTHERS;
            }else{
                /*CAREFULLY - OVERRIDING type IDENETIFIER ON OTHER TERM*/
                switch(left_side){
                    case Int_nil:
                        (*left_token)->type = INT;
                        break;
                    case Double_nil:
                        (*left_token)->type = DOUBLE;
                        break;
                    case String_nil:
                        (*left_token)->type = STRING;
                        break;
                    default:
                    SEMANTIC_ERROR_OTHERS;
                }
            }
        }else{
            return SYNTAX_ERROR;
        }
        return SUCCESS;
    }
}

error_t expression_compose(expression_s **expression_stack, variable_type *expression_type){
    error_t error;    
    token_t *operand;
    token_t *operator;
    bst_node *id;
    token_type_t token_type;

    operand = expression_stack_pop((*expression_stack));
    operator = expression_stack_pop((*expression_stack));

    if(operator->type ==  PLUS|| operator->type ==  MINUS|| operator->type ==  MULTIPLY|| operator->type == DIVIDE){
        error = parser_expression_type_control_arithmetic_strings(operand, expression_type);
        if(error != SUCCESS){
            return error;
        }
        if(operand->type == IDENTIFIER){
            id = search_variable_in_all_scopes(stack, operand->data);
            if(id->variable_type == String){
                if(operator->type != PLUS){
                return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                }
            }   
        }else if(operand->type == STRING){
            if(operator->type != PLUS){
                return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
            }
        }
        destroy_token(operand);
        destroy_token(operator);

        operand = expression_stack_pop((*expression_stack));
        error = parser_expression_type_control_arithmetic_strings(operand, expression_type);
        if(error != SUCCESS){
            return error;
        }
        switch((*expression_type)){
            case Int:
                operand->type = INT;
                break;
            case Double:
                operand->type = DOUBLE;
                break;
            case String:
                operand->type = STRING;
                break;
        }
        (*expression_stack)->top = (*expression_stack)->top + 1;
       (*expression_stack)->token_array[(*expression_stack)->top] = operand;

    }else if(operator->type == NIL_CONVERT){
        token_t *old_operand = expression_stack_pop((*expression_stack));
        parser_expression_nil_convert(&old_operand, operand);

        (*expression_stack)->top = (*expression_stack)->top + 1;
        (*expression_stack)->token_array[(*expression_stack)->top] = old_operand;

        destroy_token(operand);
        destroy_token(operator); 

    }else{
        token_type = operator->type;

        error = parser_expression_type_control_rel_operators(operand, expression_type, token_type);
        if(error != SUCCESS){
            return error;
        }
        
        destroy_token(operand);
        destroy_token(operator);

        operand = expression_stack_pop((*expression_stack));
        error = parser_expression_type_control_rel_operators(operand, expression_type, token_type);
        if(error != SUCCESS){
            return error;
        }
        
        (*expression_stack)->top = (*expression_stack)->top + 1;
       (*expression_stack)->token_array[(*expression_stack)->top] = operand;
    }
    return SUCCESS;
}

//returns true if we want to compose
bool push_or_compose(expression_s **expression_stack, int new_operator_priority){
    int old_operator_priority;
    if((*expression_stack)->top == 0){
        return false;
    }
    token_t *token = (*expression_stack)->token_array[(*expression_stack)->top - 1];
    if(token->type == MULTIPLY || token->type == DIVIDE){
        old_operator_priority = 1;
    }else if(token->type == PLUS || token->type == MINUS){
        old_operator_priority = 2;
    }else{
        old_operator_priority = 3;
    }

    if(old_operator_priority <= new_operator_priority){
        return true;
    }else{
        return false;
    }
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////EXPRESSION////////////////////////////////////
error_t parser_expression(scanner_t *scanner, token_t *token, variable_type *control_type){
    error_t error;
    bst_node *variable;
    variable_type expression_type;
    variable_type expression_type_par;
    

    //stack for precedence
    expression_s *expression_stack = expression_stack_init();
    int priority = 5;

    //flags for decision what i need from token
    bool want_VarOrLit = true;
    bool want_operator = false;

    //processing expression
    while(true){

            error = get_token(scanner, &token);
            if(error != SUCCESS){
                return error;
            }
            
        ///////////////got (
        if((token->type == LEFT_PAR)){
            if(!want_VarOrLit){
                return SYNTAX_ERROR;
            }else{
                par_stack_push(p_stack, '(');
                error = parser_expression(scanner, token, &expression_type_par);
                if(error != SUCCESS){
                    return error;
                }
                //token simulating type of expression in paranthesis
                token_t *simulation = init_token(expression_type_par); 
                expression_stack_push(expression_stack, simulation);
                bool want_VarOrLit = false;
                bool want_operator = true;
            }

        ///////////////got * / 
        }else if(token->type == MULTIPLY || token->type == DIVIDE){
            
            if(!want_operator){
                return SYNTAX_ERROR;
            }else{
                if(priority <= 1){
                    error = expression_compose(&expression_stack, &expression_type);
                    if(error != SUCCESS){
                        return error;
                    }
                    while(true){
                        bool push_comp = push_or_compose(&expression_stack, 1);
                        if(push_comp){
                            error = expression_compose(&expression_stack, &expression_type);
                            if(error != SUCCESS){
                                return error;
                            }
                        }else{
                            priority = 1;
                            expression_stack_push(expression_stack, token);
                            break;
                        }
                    }
                }else{
                    priority = 1;
                    expression_stack_push(expression_stack, token);
                }
                printf("%s\n", expression_stack->token_array[expression_stack->top]->data);
                want_VarOrLit = true;
                want_operator = false; 
            }
        
        ///////////////got + -
        }else if(token->type == PLUS || token->type == MINUS){
            if(!want_operator){
                return SYNTAX_ERROR;
            }else{
                if(priority <= 2){
                    error = expression_compose(&expression_stack, &expression_type);
                    if(error != SUCCESS){
                        return error;
                    }
                    while(true){
                        bool push_comp = push_or_compose(&expression_stack, 2);
                        if(push_comp){
                            error = expression_compose(&expression_stack, &expression_type);
                            if(error != SUCCESS){
                                return error;
                            }
                        }else{
                            priority = 2;
                            expression_stack_push(expression_stack, token);
                            break;
                        }
                    }
                }else{
                    priority = 2;
                    expression_stack_push(expression_stack, token);
                }
                printf("%s\n", expression_stack->token_array[expression_stack->top]->data);
                want_VarOrLit = true;
                want_operator = false;
            }
        
        ///////////////got == != < <= > >=
        }else if(token->type == EQUALS || token->type == LESS || token->type == MORE ||
                token->type == MORE_EQUALS || token->type == NOT_EQUALS || token->type == LESS_EQUALS){
            
            if(!want_operator){
                return SYNTAX_ERROR;
            }else{
                if(priority <= 3){
                    error = expression_compose(&expression_stack, &expression_type);
                    if(error != SUCCESS){
                        return error;
                    }
                    while(true){
                        bool push_comp = push_or_compose(&expression_stack, 3);
                        if(push_comp){
                            error = expression_compose(&expression_stack, &expression_type);
                            if(error != SUCCESS){
                                return error;
                            }
                        }else{
                            priority = 3;
                            expression_stack_push(expression_stack, token);
                            break;
                        }
                    }
                }else{
                    priority = 3;
                    expression_stack_push(expression_stack, token);
                }

                printf("%s\n", expression_stack->token_array[expression_stack->top]->data);
                want_VarOrLit = true;
                want_operator = false;
            }

        ///////////////got !
        }else if(token->type == EXCLAMATION){
            if(!want_operator){
                return SYNTAX_ERROR;
            }else{
                if(expression_stack->token_array[expression_stack->top]->type == IDENTIFIER){
                    variable = search_variable_in_all_scopes(stack, expression_stack->token_array[expression_stack->top]->data);
                    switch(variable->variable_type){
                        /*CAREFULLY - OVERRIDING type IDENETIFIER ON OTHER TERM*/
                        case Int_nil:
                            expression_stack->token_array[expression_stack->top]->type = INT;
                        case Double_nil:
                            expression_stack->token_array[expression_stack->top]->type = DOUBLE;
                        case String_nil:
                            expression_stack->token_array[expression_stack->top]->type = STRING;
                        default:
                            return SEMANTIC_ERROR_OTHERS;
                    }
                }else{
                    return SYNTAX_ERROR;
                }
                want_VarOrLit = false;
                want_operator = true;
            }

        ///////////////got ??
        }else if(token->type == NIL_CONVERT){
            
            if(!want_operator){
                return SYNTAX_ERROR;
            }else{
                if(priority <= 4){
                    error = expression_compose(&expression_stack, &expression_type);
                    if(error != SUCCESS){
                        return error;
                    }
                    while(true){
                        bool push_comp = push_or_compose(&expression_stack, 4);
                        if(push_comp){
                            error = expression_compose(&expression_stack, &expression_type);
                            if(error != SUCCESS){
                                return error;
                            }
                        }else{
                            priority = 4;
                            expression_stack_push(expression_stack, token);
                            break;
                        }
                    }
                }else{
                    priority = 4;
                    expression_stack_push(expression_stack, token);
                }
               
                want_VarOrLit = true;
                want_operator = false;    
            }

        ///////////////got IDENTIFIER STRING INT DOUBLE
        }else if(token->type == IDENTIFIER || token->type == STRING ||
                token->type ==  INT || token->type == DOUBLE){
            if(!want_VarOrLit){
                return SYNTAX_ERROR;
            }else{
                if(token->type == IDENTIFIER){
                    variable = search_variable_in_all_scopes(stack, token->data);
                    //variable exists?
                    if(variable == NULL){
                        return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
                    }
                    //variable is init ?
                    if(variable->data == NULL){     
                        return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
                    }
                }
                
                expression_stack_push(expression_stack, token);
                printf("%s\n", expression_stack->token_array[expression_stack->top]->data);    
                want_VarOrLit = false;
                want_operator = true;          
            }
        }else{
            if(token->type == RIGHT_PAR || token->type == NEW_LINE){
                if(token->type == RIGHT_PAR){
                    par_stack_pop(p_stack);
                }
                break;
            }else{
               
                printf("%d\n", token->type);
                return SYNTAX_ERROR;
            }
        }
    }//while
    if(want_VarOrLit == true){
        return SYNTAX_ERROR;
    }  

    //pop the expression stack
    while(true){
        error = expression_compose(&expression_stack, &expression_type);
        if(error != SUCCESS){
            return error;
        }
        if(expression_stack->top == 0){
            break;
        }
    }
    return SUCCESS;   
}


error_t parser_function(scanner_t *scanner, token_t *token){
    error_t error;
    error = get_token(scanner, &token);

    if(token->type  != IDENTIFIER){
        return SYNTAX_ERROR;
    }

    error = get_token(scanner, &token);
    if(token->type != LEFT_PAR){
        return SYNTAX_ERROR;
    }

    //kontrola argumentu funkce
    error = parser_argument(scanner, token);

    if(error != SUCCESS){
        return SYNTAX_ERROR;
    }

    //kontrola navratoveho typu
    error = parser_return_type(scanner,token);
    if(error != SUCCESS){
        return SYNTAX_ERROR;
    }

    return SUCCESS;
}

error_t parser_argument(scanner_t *scanner, token_t *token){
    error_t error;

    while(token->type != RIGHT_PAR){
        error = get_token(scanner, &token);
        if(token->type != IDENTIFIER){
            return SYNTAX_ERROR;
        }
        error = get_token(scanner, &token);
        if(token->type != IDENTIFIER){
            return SYNTAX_ERROR;
        }
        error = get_token(scanner, &token);
        if(token->type != COLON){
            return SYNTAX_ERROR;
        }
        error = get_token(scanner, &token);
        if(token->type != KEYWORD){
            return SYNTAX_ERROR;
        }
        error = get_token(scanner, &token);
        if(token->type == COMMA){
            //pocet argumentu vice nez 1 
        }
    }

    return SUCCESS;
}

error_t parser_return_type(scanner_t *scanner, token_t *token){
    error_t error;

    error = get_token(scanner, &token);
    if(token->type != RETURN_TYPE){
        return SYNTAX_ERROR;
    }
    error = get_token(scanner, &token);
    if(token->type != KEYWORD){
        return SYNTAX_ERROR;
    }
    error = get_token(scanner, &token);
    if(token->type != LEFT_BR){
        return SYNTAX_ERROR;
    }
    return SUCCESS;
}


#endif
