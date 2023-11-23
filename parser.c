
#include "parser.h"

#ifndef PARSER_C
#define PARSER_C 

scope_stack *stack;//symtable
par_stack *p_stack;
sym_t_function *fun_calls;
int fun_calls_num;

void add_fun_call(sym_t_function *funcall){
    if(fun_calls_num == 0){
        fun_calls = malloc(sizeof(sym_t_function));
    }else{
        fun_calls = realloc(fun_calls, sizeof(sym_t_function) * (fun_calls_num + 1));
    }
    fun_calls[fun_calls_num] = *funcall;
    fun_calls_num += 1;
}

void dispose_funcalls(){
    for(int i = 0; i < fun_calls_num; i++){
        sym_t_function *fun = &fun_calls[i];
        free(fun->id);

        for(int j = 0; j < fun->num_params; j++){
            free(fun->params[j].param_name);
            free(fun->params[j].param_id);
        }
        if(fun->num_params != 0){
            free(fun->params);
        }
    }
    if(fun_calls_num != 0){
        free(fun_calls);
    }
    fun_calls_num = 0;
}

//token for decision if i got next expression or not
//token_t *next_token = NULL;

void init_parser(){
    stack = scope_stack_init();
    p_stack = par_stack_init();
    fun_calls_num = 0;
    fun_calls = NULL;
}

void free_parser(){
    stack_dispose(&stack);
    par_stack_dispose(&p_stack);
    dispose_funcalls();
    
}



error_t run_parser(scanner_t *scanner){
    token_t *token;
    error_t error;
    //musim inicializovat globalni scope
    scope_stack_push(stack);
    while(true){
        //checking if i didn't start next expression in expression before
        //if(next_token == NULL){
            error = get_token(scanner, &token);

            //checking error from scanner
            if(error != SUCCESS){
                destroy_token(token);
                return error;
            }

            //we were in if or while or func body
            if(token->type == RIGHT_BR){
                //end of local scope
                error = scope_stack_pop(stack);
                if(error != SUCCESS){
                    return error;
                }

                //search for }
                if(p_stack->par_stack_array[p_stack->top] != '{'){
                    return SYNTAX_ERROR;
                }
                error = par_stack_pop(p_stack);
                if(error != SUCCESS){
                    return error;
                }
                return SUCCESS;
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
       /* }else{
            token_t *tmp;
            tmp =  next_token;
            next_token = NULL;

            //end of file for compilation
            if(tmp->type == EOF_TYPE){
                destroy_token(tmp);
                break;
            }

            //start of checking which expression i got
            error = parser_analyse(scanner, tmp); 

            if(error != SUCCESS){
                printf("chyba %d \n", error);
                // todo
                // destroy_token(tmp);
                return error;
            }
            
            destroy_token(tmp);
        }*/
    }
    print_funcall();
    error = fun_calls_handler();
    if(error != SUCCESS){
        printf("chyba funcall %d \n", error);
        return error;
    }

    //cotrol for paranthesis
    if(p_stack->top != -1){
        printf("paranth\n");
        return SYNTAX_ERROR;
    }

    printf("succes\n");
    return SUCCESS;
}

//decision for which expression we are going
error_t parser_analyse(scanner_t *scanner, token_t *token){
    char *func_name;
    error_t error;

    switch (token->type)
    {
        case KEYWORD:
            if(strcmp(token->data, "let") == 0){
                return parser_variable(scanner, token);
            }else if(strcmp(token->data, "var") == 0){
                return parser_variable(scanner, token);
            }else if(strcmp(token->data, "func") == 0){
                return parser_function(scanner, token);
            }else if(strcmp(token->data, "if") == 0){
                return parser_if_or_while_statement(scanner, token, true);
            }else if(strcmp(token->data, "while") == 0){
                return parser_if_or_while_statement(scanner, token, false);
            }else{
                return SYNTAX_ERROR;
            }
        case NEW_LINE:
            return SUCCESS;
        case EOF_TYPE:
            return SUCCESS;         
        case IDENTIFIER:
            func_name = string_copy(token->data);
            //todo free
            error_t error = get_token(scanner, &token);
            printf("%d\n", token->type);
            if(token->type == LEFT_PAR){
                return parser_function_call(scanner, func_name, Not_specified);
            }else{
                return parser_def_or_dec_variable(scanner, token, func_name);
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
        if(strcmp(token->data, "String") == 0 || strcmp(token->data, "Int") == 0 || strcmp(token->data, "Double") == 0 || strcmp(token->data, "String?") == 0 || strcmp(token->data, "Int?") == 0 || strcmp(token->data, "Double?") == 0){
            return SUCCESS;
        }else{
            return SYNTAX_ERROR;
        }
    }else{
        return SYNTAX_ERROR;
    }
}
/////////////////////////////////////////////////////////////////////////////

///////////////////////////NEW VARIABLE /////////////////////////////////////
error_t parser_variable(scanner_t *scanner, token_t *token){
    bool can_modify;
    if(strcmp(token->data, "let") == 0){
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

error_t parser_variable_identifier(scanner_t *scanner, token_t *token, bool can_modify){
    error_t error;
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
        identifier = current_scope(stack);
        bst_insert(&identifier, token->data, node_data_type);
        identifier = bst_search(identifier, token->data);
    }else{
        return SEMANTIC_ERROR_UNDEF_FUN_OR_REDEF_VAR;
    }

    error = parser_variable_type_and_data(scanner, token, identifier);
    if(error != SUCCESS){
        return error;
    }
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
            if(token->type != NEW_LINE){
                return SYNTAX_ERROR;
            }else{
                return SUCCESS;
            }
        }

    }
    
    //there is no data type 
    if(valid_expression == false){
        tree_node->variable_type = Not_specified;
    }

    //next is assignment
    if(parser_assignment(token) == SUCCESS){
        valid_expression = true;

        variable_type *type_of_variable = (variable_type*)malloc(sizeof(variable_type));
        bool if_while_condition = false;
        error = parser_expression(scanner, token, type_of_variable, &if_while_condition, false, NULL);
        if(error != SUCCESS){

            return error;
        }

        if(tree_node->variable_type == Not_specified){
            if((*type_of_variable) == Nil){
                return SEMANTIC_ERROR_TYPE_CANNOT_INFERRED;
            }
            tree_node->variable_type = (*type_of_variable);    
        }else{
            //control if var type and expression type are same
            if(tree_node->variable_type == Int_nil || tree_node->variable_type == Double_nil || tree_node->variable_type == String_nil){
                if(tree_node->variable_type == Int_nil && ((*type_of_variable) != Int && (*type_of_variable) != Int_nil && (*type_of_variable) != Nil)){
                    return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                }else if(tree_node->variable_type == Double_nil && ((*type_of_variable) == String || (*type_of_variable) == String_nil)){
                    return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                }else if(tree_node->variable_type == String_nil && ((*type_of_variable) != String && (*type_of_variable) != String_nil && (*type_of_variable) != Nil)){
                    return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                }
            }
            if(tree_node->variable_type == Int || tree_node->variable_type == Double || tree_node->variable_type == String){
                if(tree_node->variable_type == Double){
                    if((*type_of_variable) == String){
                        return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                    }
                }else{
                    if(tree_node->variable_type != (*type_of_variable)){
                        return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                    }
                }
            }
        }
        ////////TO DO ONLY SIMULATION OF DATA TO VARIABLE
            if((*type_of_variable) == String || (*type_of_variable) == String){
                insert_variable_data(tree_node, "simulation");
            }else if((*type_of_variable) == Int || (*type_of_variable) == Int_nil){
                insert_variable_data(tree_node, "111");
            }else if((*type_of_variable) == Double || (*type_of_variable) == Double_nil){
                insert_variable_data(tree_node, "111.1");
            }
        free(type_of_variable);
        
        if(error != SUCCESS){
            return error;
        }
    }
    //control if i got at least type or value
    if(valid_expression == true){
        return SUCCESS;
    }else{
        return SYNTAX_ERROR;
    }
}

error_t parser_def_or_dec_variable(scanner_t *scanner, token_t *token, char *var_name){
    error_t error;
    bst_node *variable = search_variable_in_all_scopes(stack, var_name);
    if(variable == NULL){
        return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
    }
    if(variable->node_data_type == VARIABLE_LET){
        return SEMANTIC_ERROR_OTHERS;
    }
    if(token->type != ASSIGMENT){
        return SYNTAX_ERROR;
    }
    variable_type *type_of_variable = (variable_type*)malloc(sizeof(variable_type));
    bool if_while_condition = false;
    error = parser_expression(scanner, token, type_of_variable, &if_while_condition, false, NULL);
    if(error != SUCCESS){
        return error;
    }

    if(variable->variable_type == Not_specified){
        if((*type_of_variable) == Nil){
            return SEMANTIC_ERROR_TYPE_CANNOT_INFERRED;
        }
        variable->variable_type = (*type_of_variable);    
    }else{
        //control if var type and expression type are same
        if(variable->variable_type == Int_nil || variable->variable_type == Double_nil || variable->variable_type == String_nil){
            if(variable->variable_type == Int_nil && ((*type_of_variable) != Int && (*type_of_variable) != Int_nil && (*type_of_variable) != Nil)){                
                return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
            }else if(variable->variable_type == Double_nil && ((*type_of_variable) == String || (*type_of_variable) == String_nil)){
                return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
            }else if(variable->variable_type == String_nil && ((*type_of_variable) != String && (*type_of_variable) != String_nil && (*type_of_variable) != Nil)){
                return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
            }
        }

        if(variable->variable_type == Int || variable->variable_type == Double || variable->variable_type == String){
            if(variable->variable_type == Double){
                if((*type_of_variable) == String){
                    return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                }
            }else{
                if(variable->variable_type != (*type_of_variable)){
                    return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                }
            }
        }
    }
    ////////TO DO ONLY SIMULATION OF DATA TO VARIABLE
        if((*type_of_variable) == String || (*type_of_variable) == String){
            insert_variable_data(variable, "simulation");
        }else if((*type_of_variable) == Int || (*type_of_variable) == Int_nil){
            insert_variable_data(variable, "111");
        }else if((*type_of_variable) == Double || (*type_of_variable) == Double_nil){
            insert_variable_data(variable, "111.1");
        }
    free(type_of_variable);
    
    if(error != SUCCESS){
        return error;
    }else{
        return SUCCESS;
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
                if(strcmp(id->data, "nil") == 0){
                    (*type_control) = Nil;
                }else{
                    (*type_control) = id->variable_type;
                }
                return SUCCESS;
            case KEYWORD:
                if(strcmp(token->data, "nil") == 0){
                    (*type_control) = Nil;
                    return SUCCESS;
                }else{
                    return SYNTAX_ERROR;
                }
            case STRING:
                (*type_control) = String;
                return SUCCESS; 
            case INT:
                (*type_control) = Int;
                return SUCCESS; 
            case DOUBLE:
                (*type_control) = Double;
                return SUCCESS;
            case DOUBLE_NIL:
                (*type_control) = Double_nil;
                return SUCCESS;
            case INT_NIL:
                (*type_control) = Int_nil;
                return SUCCESS;
            case STRING_NIL:
                (*type_control) = String_nil;
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
        if((*type_control) == Nil || (*type_control) == Int_nil || (*type_control) == String_nil || (*type_control) == Double_nil){
            return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
        }

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
            default:
                return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
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
        if((*type_control) == Nil || (*type_control) == Int_nil || (*type_control) == String_nil || (*type_control) == Double_nil){
            return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
        }

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
            printf("%dsdd\n", (*type_control));
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
                default:
                    return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
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
                default:
                    return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
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
    if((*expression_stack)->top == -1){
        switch(operand->type){
            case IDENTIFIER:
                id = search_variable_in_all_scopes(stack, operand->data);
                (*expression_type) = id->variable_type;
                (*expression_stack)->top = (*expression_stack)->top + 1;
                (*expression_stack)->token_array[(*expression_stack)->top] = operand;
                break;
            case STRING:
                (*expression_type) = String;
                (*expression_stack)->top = (*expression_stack)->top + 1;
                (*expression_stack)->token_array[(*expression_stack)->top] = operand;
                break;
            case INT:
                (*expression_type) = Int;
                (*expression_stack)->top = (*expression_stack)->top + 1;
                (*expression_stack)->token_array[(*expression_stack)->top] = operand;
                break;
            case DOUBLE:
                (*expression_type) = Double;
                (*expression_stack)->top = (*expression_stack)->top + 1;
                (*expression_stack)->token_array[(*expression_stack)->top] = operand;
                break;
            case KEYWORD:
                (*expression_type) = Nil;
                (*expression_stack)->top = (*expression_stack)->top + 1;
                (*expression_stack)->token_array[(*expression_stack)->top] = operand;
        }
        return SUCCESS;
    }
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
error_t parser_expression(scanner_t *scanner, token_t *token, variable_type *control_type, bool *if_while_condition, bool is_it_while_or_if, token_t **token_to_pass){
    
    error_t error;
    bst_node *variable;
    variable_type *expression_type;
    variable_type *expression_type_par;
    
    expression_type = (variable_type*)malloc(sizeof(variable_type));
    expression_type_par = (variable_type*)malloc(sizeof(variable_type));
    
    //stack for precedence
    expression_s *expression_stack = expression_stack_init();
    int priority = 5;

    //flags for decision what i need from token
    bool want_VarOrLit = true;
    bool want_operator = false;
    bool at_least_one_operand = false;

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
                at_least_one_operand = true;

                par_stack_push(p_stack, '(');

                error = parser_expression(scanner, token, expression_type_par, if_while_condition, false, NULL);
                if(error != SUCCESS){
                    return error;
                }
                //token simulating type of expression in paranthesis
                token_type_t type_for_sim_token;
                switch((*expression_type_par)){
                        case Nil:
                            type_for_sim_token = NIL;
                            break;
                        case String:
                            type_for_sim_token = STRING;
                            break;
                        case String_nil:
                            type_for_sim_token = STRING_NIL;
                            break;
                        case Int:
                            type_for_sim_token = INT;
                            break;
                        case Int_nil:
                            type_for_sim_token = INT_NIL;
                            break;
                        case Double:
                            type_for_sim_token = DOUBLE;
                            break;
                        case Double_nil:
                            type_for_sim_token = DOUBLE_NIL;
                            break;
                    }
                token_t *simulation = init_token(type_for_sim_token); 
                expression_stack_push(expression_stack, simulation);
                free(simulation);

                want_VarOrLit = false;
                want_operator = true;
            }

        ///////////////got * / 
        }else if(token->type == MULTIPLY || token->type == DIVIDE){
            
            if(!want_operator){
                return SYNTAX_ERROR;
            }else{
                if(priority <= 1){
                    error = expression_compose(&expression_stack, expression_type);
                    if(error != SUCCESS){
                        return error;
                    }
                    while(true){
                        bool push_comp = push_or_compose(&expression_stack, 1);
                        if(push_comp){
                            error = expression_compose(&expression_stack, expression_type);
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
                    error = expression_compose(&expression_stack, expression_type);
                    if(error != SUCCESS){
                        return error;
                    }
                    while(true){
                        bool push_comp = push_or_compose(&expression_stack, 2);
                        if(push_comp){
                            error = expression_compose(&expression_stack, expression_type);
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
            if((*if_while_condition) == true){
                return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
            }
            if(!want_operator){
                return SYNTAX_ERROR;
            }else{
                if(priority <= 3){
                    error = expression_compose(&expression_stack, expression_type);
                    if(error != SUCCESS){
                        return error;
                    }
                    while(true){
                        bool push_comp = push_or_compose(&expression_stack, 3);
                        if(push_comp){
                            error = expression_compose(&expression_stack, expression_type);
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
                (*if_while_condition) = true;
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
                    error = expression_compose(&expression_stack, expression_type);
                    if(error != SUCCESS){
                        return error;
                    }
                    while(true){
                        bool push_comp = push_or_compose(&expression_stack, 4);
                        if(push_comp){
                            error = expression_compose(&expression_stack, expression_type);
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
                     
            //control for assignment to variable
            at_least_one_operand = true;
            
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
        }else if(token->type == KEYWORD){
            at_least_one_operand = true;
            if(!want_VarOrLit){
                return SYNTAX_ERROR;
            }else{
                if(strcmp(token->data, "nil") == 0){
                    expression_stack_push(expression_stack, token);
                    want_VarOrLit = false;
                    want_operator = true;  
                }else{
                    return SYNTAX_ERROR;
                }
                
            }
        }else if(token->type == LEFT_BR){
            if(is_it_while_or_if == true){
                if(at_least_one_operand == false){
                    return SYNTAX_ERROR;
                }
                (*token_to_pass) = token;
                want_VarOrLit = false;
                want_operator = true;
                break;
            }else{
                return SYNTAX_ERROR;
            }
        }else{
            if(token->type == RIGHT_PAR || token->type == NEW_LINE || token->type == EOF_TYPE){
                if(at_least_one_operand == false){
                    return SYNTAX_ERROR;
                }
                if(token->type == RIGHT_PAR){
                    error = par_stack_pop(p_stack);
                    if(error != SUCCESS){
                       return error;
                    }
                    want_VarOrLit = false;
                    want_operator = true;
                }
                break;
                
            }else{
                return SYNTAX_ERROR;
            }
        }
    }//while
    
    if(want_VarOrLit == true){
        return SYNTAX_ERROR;
    }  

    //syntax let on if or while statement
    if(expression_stack->top == 0){
        variable = search_variable_in_all_scopes(stack, expression_stack->token_array[expression_stack->top]->data);
        if(variable != NULL){
            if(variable->node_data_type == VARIABLE_LET){
                (*if_while_condition) = true;
                //TO DOO
                //next_token  = init_token_data(IDENTIFIER, variable->key, strlen(variable->key));
            }
        }
    }

    //pop the expression stack
    while(true){
        error = expression_compose(&expression_stack, expression_type);

        if(error != SUCCESS){
            return error;
        }
        if(expression_stack->top == 0){
            break;
        }
    } 

    variable_type type_to_pass = (*expression_type);
    if(control_type != NULL){
        (*control_type) = type_to_pass;
    }

    free(expression_type);
    free(expression_type_par);
    
    return SUCCESS;   
}


error_t parser_function(scanner_t *scanner, token_t *token){
    error_t error;
    sym_t_function *function = (sym_t_function*)malloc(sizeof(sym_t_function));
    function->num_params = 0;

    
    error = get_token(scanner, &token);
    if(token->type  != IDENTIFIER){
        return SYNTAX_ERROR;
    }


    bst_node *identifier = bst_search(stack->stack_array[0], token->data);
    if(identifier != NULL){
        printf("definovany\n");
        return SEMANTIC_ERROR_UNDEF_FUN_OR_REDEF_VAR; //TODO
    }
    function->id = malloc(strlen(token->data) + 1);
    strcpy(function->id, token->data);


    error = get_token(scanner, &token);
    if(token->type != LEFT_PAR){
        return SYNTAX_ERROR;
    }

    // //kontrola argumentu funkce
    error = parser_argument(scanner, token, function);

    if(error != SUCCESS){
        return error;
    }

    //kontrola navratoveho typu
    error = parser_return_type(scanner, token, function);
    if(error != SUCCESS){
        return error;
    }
    // bst_node *tree_node = current_scope(stack);
    bst_node *tree_node = stack->stack_array[0];
    insert_function(&tree_node, function->id, function);
    printf("inserted address %p\n", function);
    bst_print(stack->stack_array[0]);
    printf("function id: %s\n", function->id);
    printf("stack top %d\n", stack->top);
    printf("tree node address %p\n", stack->stack_array[0]);
    //vyhodnoceni tela funkce
    // error = parser_return(scanner, token);
    // printf("dfas\n");
    error = get_token(scanner, &token);
    if(token->type != KEYWORD){
        // if(token->data != 'return'){
            // return SYNTAX_ERROR;
        // }
        return SYNTAX_ERROR;
    }

    error = get_token(scanner, &token);
    if(token->type != RIGHT_BR){
        return SYNTAX_ERROR;
    }
    return SUCCESS;
}

error_t parser_argument(scanner_t *scanner, token_t *token, sym_t_function *struktura){
    error_t error;
    bst_node* arg_tree;
    bst_init(&arg_tree);

    error = get_token(scanner, &token);
    if(token->type != RIGHT_PAR){
    while(1){
        if(struktura->num_params == 0){
            struktura->params = (sym_t_param*)malloc(sizeof(sym_t_param));
        } else {
            struktura->params = (sym_t_param*)realloc(struktura->params,sizeof(sym_t_param) * (struktura->num_params + 1));
        }
        if(token->type != IDENTIFIER){
            return SYNTAX_ERROR;
        }

        struktura->params[struktura->num_params].param_name = malloc(strlen(token->data) + 1);
        strcpy(struktura->params->param_name, token->data);
        error = get_token(scanner, &token);
        if(token->type != IDENTIFIER){
            return SYNTAX_ERROR;
        }
        if(bst_search(arg_tree, token->data) != NULL){
            return SEMANTIC_ERROR_OTHERS;
        }
        bst_insert(&arg_tree, token->data, FUNCTION);

        struktura->params[struktura->num_params].param_id = malloc(strlen(token->data) + 1);
        strcpy(struktura->params->param_id, token->data);

        error = get_token(scanner, &token);
        if(token->type != COLON){
            return SYNTAX_ERROR;
        }
        error = get_token(scanner, &token);
        if(token->type != KEYWORD){
            return SYNTAX_ERROR;
        }

        variable_type param_type = find_variable_type(token->data);
        if(param_type == Not_specified){
            return SYNTAX_ERROR;
        }
        struktura->params->param_type = param_type;
        struktura->num_params += 1;

        error = get_token(scanner, &token);
        if(token->type == COMMA){
            // printf("mam carku\n");
            //pocet argumentu vice nez 1
        error = get_token(scanner, &token);

        } else if(token->type == RIGHT_PAR){
            break;
        } else {
            return SYNTAX_ERROR;
        }

    }

    }
    bst_dispose(&arg_tree);
    return SUCCESS;
}

error_t parser_return_type(scanner_t *scanner, token_t *token, sym_t_function *struktura){
    error_t error;
    
    error = get_token(scanner, &token);
    if(token->type != RETURN_TYPE){
        struktura->return_type = Void;
        // printf("typ: %d\n", struktura->return_type);
        // return SYNTAX_ERROR;
        // return SUCCESS;
    } else {
        
        error = get_token(scanner, &token);
        if(token->type != KEYWORD){
            return SYNTAX_ERROR;
        }

        variable_type variable = find_variable_type(token->data);
        struktura->return_type = variable;

        error = get_token(scanner, &token);
        if(token->type != LEFT_BR){
            return SYNTAX_ERROR;
        }
    }
    return SUCCESS;
}

error_t parser_return(scanner_t *scanner, token_t *token){
    error_t error;
    
    error = get_token(scanner, &token);
    if(token->data != "return"){
        return SYNTAX_ERROR;
    }
    error = get_token(scanner, &token);
    if(token->type != RIGHT_BR){
        return SYNTAX_ERROR;
    }
    //TODO
    //zavolame vyhodnoceni vyrazu
    return SUCCESS;
}
 // if(error != 0){
//     return error;
// }
#define CHECKERROR(error) if ((error) != 0) return error; 

error_t parser_function_call(scanner_t *scanner, char* func_name, variable_type required_return_type){
    error_t error;
    token_t *token;
    sym_t_function fun_call;
    fun_call.return_type = required_return_type;
    fun_call.num_params = 0;
    fun_call.id = string_copy(func_name);
    error = get_token(scanner, &token);
    CHECKERROR(error)
    if(token->type != RIGHT_PAR){
        while (1)
        {
            if(fun_call.num_params == 0){
                fun_call.params = (sym_t_param*)malloc(sizeof(sym_t_param));
            } else {
                fun_call.params = (sym_t_param*)realloc(fun_call.params,sizeof(sym_t_param) * (fun_call.num_params + 1));
            }
            fun_call.params[fun_call.num_params].param_name = NULL;
            fun_call.params[fun_call.num_params].param_id = NULL;

            if((token->type == IDENTIFIER) ||(token->type == STRING)||(token->type == INT)||(token->type == DOUBLE)||(token->type == NIL)){
                
                if(token->type == IDENTIFIER){
                    char *first_param_token = string_copy(token->data);
                    error = get_token(scanner, &token);
                    CHECKERROR(error)
                    if(token->type == COLON){
                        fun_call.params[fun_call.num_params].param_name = first_param_token;
                        error = get_token(scanner, &token);
                        CHECKERROR(error)
                        if((token->type == IDENTIFIER) ||(token->type == STRING)||(token->type == INT)||(token->type == DOUBLE)||(token->type == NIL)){
                            variable_type vartype;
                            if(token->type == STRING){
                                vartype = String;
                            } else if(token->type == INT){
                                vartype = Int;
                            } else if(token->type == DOUBLE){
                                vartype = Double;
                            } else if(token->type == NIL){
                                vartype = Nil;
                            } else if(token->type == IDENTIFIER){
                                bst_node *id = search_variable_in_all_scopes(stack, token->data);
                                if(id == NULL){
                                    return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
                                }
                                if(id->node_data_type == FUNCTION){
                                    //todo error
                                    return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
                                }
                                vartype = id->variable_type;
                            }
                            fun_call.params[fun_call.num_params].param_type = vartype;
                            error = get_token(scanner, &token);
                            CHECKERROR(error)
                        }else{
                            return SYNTAX_ERROR;
                        }
                    } else {
                        bst_node *id = search_variable_in_all_scopes(stack, first_param_token);
                        if(id == NULL){
                            return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
                        }
                        if(id->node_data_type == FUNCTION){
                            //todo error
                            return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
                        }
                        free(first_param_token);

                        fun_call.params[fun_call.num_params].param_type = id->variable_type;
                    }
                }else{
                    variable_type vartype;
                    if(token->type == STRING){
                        vartype = String;
                    } else if(token->type == INT){
                        vartype = Int;
                    } else if(token->type == DOUBLE){
                        vartype = Double;
                    } else if(token->type == NIL){
                        vartype = Nil;
                    }
                    fun_call.params[fun_call.num_params].param_type = vartype;
                    error = get_token(scanner, &token);
                    CHECKERROR(error)
                }
            } else {
                return SYNTAX_ERROR;
            }
            fun_call.num_params++;
            if(token->type == COMMA){
                error = get_token(scanner, &token);
                CHECKERROR(error)
            } else if(token->type == RIGHT_PAR){
                break;
            } else {
                return SYNTAX_ERROR;
            }
        }
        
    }
        add_fun_call(&fun_call);
      return SUCCESS;

    }

bool check_type_compatibility(variable_type def, variable_type val){
    if(def == val){
        return true;
    }
    if(def == Not_specified){
        return true;
    }
    if(def == String_nil && val == String){
        return true;
    }
    if(def == Int_nil && val == Int){
        return true;
    }
    if(def == Double_nil && val == Double){
        return true;
    }

}

error_t fun_calls_handler(){
    for(int i = 0; i < fun_calls_num; i++){
        bst_node *node = bst_search(stack->stack_array[0], fun_calls[i].id);
        if(node == NULL){
            return SEMANTIC_ERROR_UNDEF_FUN_OR_REDEF_VAR;
        }
        if(node->node_data_type != FUNCTION){
            return SEMANTIC_ERROR_UNDEF_FUN_OR_REDEF_VAR;
        }
        sym_t_function *node_fun = node->data;
        if(node_fun->num_params != fun_calls[i].num_params){
            //todo
            return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
        }
        printf("a\n");
        for(int j = 0; j < fun_calls[i].num_params; j++){
            if(fun_calls[i].params[j].param_name != NULL){
                if(strcmp(node_fun->params[j].param_name, fun_calls[i].params[j].param_name) != 0){
                    return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
                }
            }else {
                if(strcmp(node_fun->params[j].param_name, "_") != 0){
                    return SEMANTIC_ERROR_TYPE_CANNOT_INFERRED;
                }
            }
            if(!check_type_compatibility(node_fun->params[j].param_type, fun_calls[i].params[j].param_type)){
                return SEMANTIC_ERROR_TYPE_CANNOT_INFERRED;
            }
        }
        printf("return type call: %s, def: %s\n", variable_type_to_str(fun_calls[i].return_type), variable_type_to_str(node_fun->return_type));
        if(!check_type_compatibility(fun_calls[i].return_type, node_fun->return_type)){
            return SEMANTIC_ERROR_TYPE_CANNOT_INFERRED;
        }
    }
    return SUCCESS;
}



void print_funcall(){
    printf("Printing fun calls:\n");
    for(int i = 0; i < fun_calls_num; i++){
        printf("%s(", fun_calls[i].id);
        for(int j = 0; j < fun_calls[i].num_params; j++){
            if(fun_calls[i].params[j].param_name != NULL){
                printf("%s:", fun_calls[i].params[j].param_name);
            }

            printf("%s   ", variable_type_to_str(fun_calls[i].params[j].param_type));

        }
        printf(")\n");
    }

}


error_t parser_if_or_while_statement(scanner_t *scanner, token_t *token, bool if_or_while){
    error_t error;
    bool if_while = false;
    token_t *token_to_pass;
    token_to_pass = (token_t*)malloc(sizeof(token_t));
    //token_t *let_syntax;
    
    //TO DO NEW LINES BETWEEN IF AND ()
    
    error = parser_expression(scanner, token, Not_specified, &if_while, true, &token_to_pass);

    if(error != SUCCESS){
        return error;
    }
    if(if_while == false){
        return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
    }

    //let syntax problem
    //if(next_token != NULL){
    //    let_syntax = init_token_data(IDENTIFIER, next_token->data, strlen(next_token->data));
    //    destroy_token(next_token);
    //}
    //type must be not nil 
    // TO DOO
    //destroy_token(let_syntax);


    
    error = parser_if_or_while_body(scanner, token_to_pass);
    if(error != SUCCESS){
        return error;
    }
    //if - true || while - false
    if(if_or_while){
        while(true){
            error = get_token(scanner, &token);
            if(error != SUCCESS){
                return error;
            }

            if(token->type == NEW_LINE){
                continue;
            }else if(token->type == KEYWORD){
                if(strcmp(token->data, "else") == 0){
                    break;
                }else{
                    return SYNTAX_ERROR;
                }
            }else{
                return SYNTAX_ERROR;
            }
        }

        error = parser_if_or_while_body(scanner, token);
        if(error != SUCCESS){
            return error;
        }
    }
    return SUCCESS;
}

error_t parser_if_or_while_body(scanner_t *scanner, token_t *token){
    error_t error;

    while(true){
        //left BR from expression
        if(token->type == LEFT_BR){
            break;
        }
        error = get_token(scanner, &token);
        if(error != SUCCESS){
            return error;
        }

        if(token->type == NEW_LINE){
            continue;
        }
        else if(token->type == LEFT_BR){
            break;
        }else{
            return SYNTAX_ERROR;
        }
    }

    // { push
    par_stack_push(p_stack, '{');

    //local scope
    scope_stack_push(stack);

    //work in local scope
    error = run_parser(scanner);
    if(error != SUCCESS){
        return error;
    }
    
    return SUCCESS;
}

#endif