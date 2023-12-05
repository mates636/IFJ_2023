
#include "parser.h"

#ifndef PARSER_C
#define PARSER_C 
#define CHECKERROR(error) if ((error) != 0) return error; 

scope_stack *stack; //symtable
par_stack *p_stack;
sym_t_function *fun_calls;
int fun_calls_num;
sym_t_function *current_function;

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

// void init_builtin_function(){
//     sym_t_function *function = (sym_t_function*)malloc(sizeof(sym_t_function));
//     function->num_params = 0;
//     function->id = string_copy("readString");
//     function->return_type = String_nil;
    
//     bst_node *tree_node = stack->stack_array[0];
//     insert_function(&tree_node, function->id, function);

//     function = (sym_t_function*)malloc(sizeof(sym_t_function));
//     function->num_params = 0;
//     function->id = string_copy("readInt");
//     function->return_type = Int_nil;
    
//     tree_node = stack->stack_array[0];
//     insert_function(&tree_node, function->id, function);

//     function = (sym_t_function*)malloc(sizeof(sym_t_function));
//     function->num_params = 0;
//     function->id = string_copy("readDouble");
//     function->return_type = Double_nil;
    
//      tree_node = stack->stack_array[0];
//     insert_function(&tree_node, function->id, function);
// }

void init_parser(){
    stack = scope_stack_init();
    p_stack = par_stack_init();
    fun_calls_num = 0;
    fun_calls = NULL;
    current_function = NULL;
   //init_builtin_function();
}

error_t free_parser(){
        //cotrol for paranthesis
    if(p_stack->top != -1){
        // printf("paranth\n");
        return SYNTAX_ERROR;
    }

    stack_dispose(&stack);
    par_stack_dispose(&p_stack);
    dispose_funcalls();
    
}


    
error_t run_parser(scanner_t *scanner, token_t *token){
    error_t error;
    while(true){
            error = get_token(scanner, &token);
            //checking error from scanner
            if(error != SUCCESS){
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
            
            //end of file for compilation or semantic error inside if/while/function body - missing }
            if(token->type == EOF_TYPE){
                break;
            }

            //start of checking which expression i got
            error = parser_analyse(scanner, token); 
            
            if(error != SUCCESS){
                // printf("chyba %d \n", error);
                return error;
            }
    }

    //print_funcall();
    error = fun_calls_handler();
    if(error != SUCCESS){
        // //printf("chyba funcall %d \n", error);
        return error;
    }

    // printf("succes\n");
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
            }else if(strcmp(token->data, "return") == 0){
                return parser_return(scanner, token);
            }else{
                return SYNTAX_ERROR;
            }
        case NEW_LINE:
            return SUCCESS;
        case COMMENT:
            return SUCCESS;
        case MULTILINE:
            return SUCCESS;
        // case IDENTIFIER:
        //     char* func_name = string_copy(token->data);
            //todo free idk
        case EOF_TYPE:
            return SUCCESS;         
        case IDENTIFIER:
            func_name = string_copy(token->data);
            //todo free
            // error_t error;

            error = get_token(scanner, &token);
            CHECKERROR(error);
            if(token->type == LEFT_PAR){
                if(is_it_built_in_function(func_name) == true){
                    return parser_built_in_function(scanner, token, func_name, NULL);
                }else{
                    return parser_function_call(scanner, func_name, Not_specified);
                }
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
            if(token->type != NEW_LINE && token->type != EOF_TYPE){
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

        error_t error;
        valid_expression = true;
        token_t *token_to_pass = (token_t*)malloc(sizeof(token_t));
        token_to_pass->type = UNKNOWN;
        variable_type *type_of_variable = (variable_type*)malloc(sizeof(variable_type));
        (*type_of_variable) = Not_specified;
        bool if_while_condition = false;

        //is it function
        error = get_token(scanner, &token);
        CHECKERROR(error);  
        if(token->type == IDENTIFIER){
            //got identifier
            size_t len = strlen(token->data) + 1;
            free(token_to_pass);
            token_to_pass = init_token_data(token->type, token->data, len);
            
            //left par for function
            error = get_token(scanner, &token);
            CHECKERROR(error);
            if(token->type == LEFT_PAR){
                error = parser_id_assignment_function(scanner, token, token_to_pass, tree_node);
                destroy_token(token_to_pass);
                return error;
            }
        }
        //it was variable - calling expression
        error = parser_expression(scanner, token, type_of_variable, &if_while_condition, false, &token_to_pass);
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
        
        //simulation of variable data value for controls
            if(tree_node->variable_type == String || tree_node->variable_type == String_nil){
                if((*type_of_variable) == Nil && tree_node->variable_type == String_nil){
                    insert_variable_data(tree_node, "nil");
                }else{
                    insert_variable_data(tree_node, "simulation");
                }
            }else if(tree_node->variable_type == Int || tree_node->variable_type == Int_nil){
                if((*type_of_variable) == Nil && tree_node->variable_type == Int_nil){
                    insert_variable_data(tree_node, "nil");
                }else{
                    insert_variable_data(tree_node, "111");
                }
            }else if(tree_node->variable_type == Double || tree_node->variable_type == Double_nil){
                if((*type_of_variable) == Nil && tree_node->variable_type == Double_nil){
                    insert_variable_data(tree_node, "nil");
                }else{
                    insert_variable_data(tree_node, "111.1");
                }
            }

        free(type_of_variable);
        free(token_to_pass);
        
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

error_t parser_id_assignment_function(scanner_t *scanner, token_t *token, token_t *function_id, bst_node *variable){
     if(is_it_built_in_function(function_id->data) == true){
        return parser_built_in_function(scanner, token, function_id->data, variable);
    }else{
        bst_node *function = search_variable_in_all_scopes(stack, function_id->data);
        variable_type fun_type =  ((sym_t_function*)function->data)->return_type;
        if(variable->variable_type == Int_nil){
            if(fun_type != Int && fun_type != Int_nil){
                return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
            }
        }else if(variable->variable_type == Double_nil){
            if(fun_type != Double && fun_type != Double_nil){
                return  SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
            }
        }else if(variable->variable_type == String_nil){
            if(fun_type != String && fun_type != String_nil){
                return  SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
            }
        }else if(variable->variable_type == Not_specified){
            variable->variable_type = fun_type;
        }else{
            if(variable->variable_type != fun_type){
                return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
            }
        }
        return parser_function_call(scanner, function_id->data, Not_specified);
    }
}

error_t parser_def_or_dec_variable(scanner_t *scanner, token_t *token, char *var_name){    
    error_t error;
    bst_node *variable = search_variable_in_all_scopes(stack, var_name);
    if(variable == NULL){
        return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
    }
    if(variable->node_data_type == VARIABLE_LET){
        if(variable->data != NULL){
            return SEMANTIC_ERROR_OTHERS;
        }
    }
    if(token->type != ASSIGMENT){
        return SYNTAX_ERROR;
    }

    token_t *token_to_pass = (token_t*)malloc(sizeof(token_t));
    token_to_pass->type = UNKNOWN;
    variable_type *type_of_variable = (variable_type*)malloc(sizeof(variable_type));
    (*type_of_variable) = Not_specified;
    bool if_while_condition = false;

    //is it function
    error = get_token(scanner, &token);
    CHECKERROR(error);  
    if(token->type == IDENTIFIER){
        //got identifier
        size_t len = strlen(token->data) + 1;
        free(token_to_pass);
        token_to_pass = init_token_data(token->type, token->data, len);
        
        //left par for function
        error = get_token(scanner, &token);
        CHECKERROR(error);
        if(token->type == LEFT_PAR){
            error = parser_id_assignment_function(scanner, token, token_to_pass, variable);
            destroy_token(token_to_pass);
            return error;
        }
    }
    //it was variable - calling expression
    error = parser_expression(scanner, token, type_of_variable, &if_while_condition, false, &token_to_pass);
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
        //simulation of variable data value for controls
        if(variable->variable_type == String || variable->variable_type == String_nil){
            if((*type_of_variable) == Nil && variable->variable_type == String_nil){
                    insert_variable_data(variable, "nil");
                }else{
                    insert_variable_data(variable, "simulation");
                }
        }else if(variable->variable_type == Int || variable->variable_type == Int_nil){
            if((*type_of_variable) == Nil && variable->variable_type == Int_nil){
                    insert_variable_data(variable, "nil");
                }else{
                    insert_variable_data(variable, "111");
                }
        }else if(variable->variable_type == Double || variable->variable_type == Double_nil){
            if((*type_of_variable) == Nil && variable->variable_type == Double_nil){
                    insert_variable_data(variable, "nil");
                }else{
                    insert_variable_data(variable, "111.1");
                }
        }
    free(type_of_variable);
    free(token_to_pass);
    
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
    sym_t_variable *var; 
    
    switch(token->type){
            case IDENTIFIER:
                id = search_variable_in_all_scopes(stack, token->data);
                var = id->data;
                if(id == NULL){
                    return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
                }else{
                    if(id->data == NULL){
                        return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
                    }
                }
                if(strcmp(var->data, "nil") == 0){
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
    // printf("%d\n", (*type_control));
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
            //printf("%dsdd\n", (*type_control));
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
    
    //type for expression control
    expression_type = (variable_type*)malloc(sizeof(variable_type));
    expression_type_par = (variable_type*)malloc(sizeof(variable_type));
    (*expression_type) = Not_specified;
    (*expression_type_par) = Not_specified;
    
    //stack for precedence
    expression_s *expression_stack = expression_stack_init();
    int priority = 5;

    //flags for decision what i need from token
    bool want_VarOrLit = true;
    bool want_operator = false;
    bool at_least_one_operand = false;

    //check if i got tokens from previous function
    int token_from_prev_fun = 0;
    token_t *tmp;
    if(token_to_pass != NULL){
        if((*token_to_pass)->type != UNKNOWN){
            //got someting in token and token_to_pass from id = ..
            token_from_prev_fun = 2;
        }else{
            //got something in token from if/while/return
            token_from_prev_fun = 1;
            tmp = token;
        }
    //printf("token_to_pass_type: %d\n", (*token_to_pass)->type);
    //printf("token_to_pass_data: %s\n", (*token_to_pass)->data);
    //printf("token_from_prev fun: %d\n", token_from_prev_fun);
    }
 
    //processing expression
    while(true){

        if(token_from_prev_fun == 2){
            tmp = token;
            token = (*token_to_pass);
            token_from_prev_fun = 1;
        }else if(token_from_prev_fun == 1){
            token = tmp;
            token_from_prev_fun = 0;
        }else{
        //getting new tokens in expression
            error = get_token(scanner, &token);
            if(error != SUCCESS){
                return error;
            }
        }

        //printf("expression_token_type: %d\n", token->type);
        //printf("expression_token_data: %s\n", token->data); 

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
                        case Void:
                            continue;
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
                //printf("%s\n", expression_stack->token_array[expression_stack->top]->data);
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
                //printf("%s\n", expression_stack->token_array[expression_stack->top]->data);
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

                //printf("%s\n", expression_stack->token_array[expression_stack->top]->data);
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
                        case Int_nil:
                            expression_stack->token_array[expression_stack->top]->type = INT;
                            expression_stack->token_array[expression_stack->top]->data = ((sym_t_variable*)variable->data)->data;
                        case Double_nil:
                            expression_stack->token_array[expression_stack->top]->type = DOUBLE;
                            expression_stack->token_array[expression_stack->top]->data = ((sym_t_variable*)variable->data)->data;
                        case String_nil:
                            expression_stack->token_array[expression_stack->top]->type = STRING;
                            expression_stack->token_array[expression_stack->top]->data = ((sym_t_variable*)variable->data)->data;
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
            if(expression_stack->top != 0){
                return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
            }
            
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
                //printf("%d\n", expression_stack->token_array[expression_stack->top]->type);    
                want_VarOrLit = false;
                want_operator = true;          
            }
         ///////////////got nil or let id syntax in if or while
        }else if(token->type == KEYWORD){
            at_least_one_operand = true;
            if(!want_VarOrLit){
                return SYNTAX_ERROR;
            }else{
                if(strcmp(token->data, "nil") == 0){
                    expression_stack_push(expression_stack, token);
                    want_VarOrLit = false;
                    want_operator = true;
                }else if(strcmp(token->data, "let") == 0){
                    if(expression_stack->top == -1){
                        (*expression_type) = Void;
                        want_VarOrLit = false;
                        want_operator = true;
                        break;
                    }else{
                        return SYNTAX_ERROR;
                    }  
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
                    if(p_stack->par_stack_array[p_stack->top] != '('){
                        return SYNTAX_ERROR;
                    }else{   
                    error = par_stack_pop(p_stack);
                    if(error != SUCCESS){
                       return error;
                    }
                    want_VarOrLit = false;
                    want_operator = true;
                }
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

    //syntax let in if or while statement 
    if(token->type == KEYWORD){
        if(strcmp(token->data, "let") == 0){
            error = get_token(scanner, &token);
                if(error != SUCCESS){
                    return error;
                }

            if(token->type != IDENTIFIER){
                return SYNTAX_ERROR;
            }else{
                variable = search_variable_in_all_scopes(stack, token->data);
                if(variable == NULL){
                    return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
                }
                if(variable->data == NULL){
                    return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
                }

                if(variable->node_data_type != VARIABLE_LET){
                    return SEMANTIC_ERROR_OTHERS;
                }
                
                (*if_while_condition) = true;
                (*token_to_pass) = token;
                return SUCCESS;
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

    expression_stack_dispose(&expression_stack);
    free(expression_type);
    free(expression_type_par);
    
    return SUCCESS;   
}



// function -> func function_id (param_name param_id : param_type) -> return_type
error_t parser_function(scanner_t *scanner, token_t *token){
    error_t error;
    sym_t_function *function = (sym_t_function*)malloc(sizeof(sym_t_function));
    function->num_params = 0;

    
    error = get_token(scanner, &token);
    CHECKERROR(error)
    if(token->type  != IDENTIFIER){
        return SYNTAX_ERROR;
    }


    bst_node *identifier = bst_search(stack->stack_array[0], token->data);
    if(identifier != NULL){
        return SEMANTIC_ERROR_UNDEF_FUN_OR_REDEF_VAR;
    }
    function->id = malloc(strlen(token->data) + 1);
    strcpy(function->id, token->data);


    error = get_token(scanner, &token);
    CHECKERROR(error)
    if(token->type != LEFT_PAR){
        return SYNTAX_ERROR;
    }

    //kontrola argumentu funkce
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
    // //printf("inserted address %p\n", function);
    //bst_print(stack->stack_array[0]);
    // //printf("function id: %s\n", function->id);
    // //printf("stack top %d\n", stack->top);
    // //printf("tree node address %p\n", stack->stack_array[0]);
    //vyhodnoceni tela funkce
    // error = parser_return(scanner, token);
    // //printf("dfas\n");
    scope_stack_push(stack);

    current_function = function;
    //SIMULATION - putting arguments into local tree for avaibility to using them in the function
    identifier = search_variable_in_all_scopes(stack, function->id);
    sym_t_function *fun = identifier->data;
    bst_node *insert_arg_as_var;
    bst_node *scope_for_insert = stack->stack_array[stack->top];

    for(int i = 0; i < fun->num_params; i++){
        char *param = fun->params[i].param_id;
        bst_insert(&scope_for_insert, param, FUNCTION_PARAM);
        identifier = bst_search(scope_for_insert, param);
        identifier->variable_type = fun->params[i].param_type;
        if(identifier->variable_type == Int || identifier->variable_type == Int_nil){
            insert_variable_data(identifier, "111");
        }else if(identifier->variable_type == Double || identifier->variable_type == Double_nil){
            insert_variable_data(identifier, "111.1");
        }else if(identifier->variable_type == String || identifier->variable_type == String_nil){
            insert_variable_data(identifier, "SIMULATION");
        }
    }
    variable_type fun_return_type = function->return_type;


    error = run_parser(scanner, token);
    
    if(error != SUCCESS){
        return error;
    }

    if(token->type == EOF_TYPE){
        return SYNTAX_ERROR;
    }

    current_function = NULL;
    scope_stack_pop(stack);

    return SUCCESS;
}


// function parameter check
// param_name para_id : param_type -> next_param
error_t parser_argument(scanner_t *scanner, token_t *token, sym_t_function *struktura){
    error_t error;
    bst_node* arg_tree;
    bst_init(&arg_tree);

    error = get_token(scanner, &token);
    CHECKERROR(error)

    if(token->type != RIGHT_PAR){
    while(1){
        if(struktura->num_params == 0){
            struktura->params = (sym_t_param*)malloc(sizeof(sym_t_param));
        } else {
            struktura->params = (sym_t_param*)realloc(struktura->params,sizeof(sym_t_param) * (struktura->num_params + 1));
        }
        //check param_name
        if(token->type != IDENTIFIER){
            return SYNTAX_ERROR;
        }

        struktura->params[struktura->num_params].param_name = malloc(strlen(token->data) + 1);
        strcpy(struktura->params->param_name, token->data);
        error = get_token(scanner, &token);
        CHECKERROR(error)
        //check param_id
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
        CHECKERROR(error)

        //check colon
        if(token->type != COLON){
            return SYNTAX_ERROR;
        }
        error = get_token(scanner, &token);
        CHECKERROR(error)

        //check param_return_type
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
        CHECKERROR(error)

        //check if there is more parameters
        if(token->type == COMMA){
        error = get_token(scanner, &token);
        CHECKERROR(error)

        //end of parameter
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


// function return_type check
// -> return_type
error_t parser_return_type(scanner_t *scanner, token_t *token, sym_t_function *struktura){
    error_t error;
    
    error = get_token(scanner, &token);
    CHECKERROR(error)

    //check return_type
    if(token->type != RETURN_TYPE){
        struktura->return_type = Void;
    } else {
        
        error = get_token(scanner, &token);
        CHECKERROR(error)

        if(token->type != KEYWORD){
            return SYNTAX_ERROR;
        }

        variable_type variable = find_variable_type(token->data);
        struktura->return_type = variable;

        error = get_token(scanner, &token);
        CHECKERROR(error)

        if(token->type != LEFT_BR){
            return SYNTAX_ERROR;
        }
    }
    par_stack_push(p_stack, '{');

    return SUCCESS;
}


// function return check
// return expression
error_t parser_return(scanner_t *scanner, token_t *token){
    error_t error;

    //check if type of expression is same as function return_type
    variable_type type_of_variable;
    bool if_while_condition = false;
    error = get_token(scanner, &token);
    CHECKERROR(error)

    if(current_function->return_type != Void){
        if(token->type == NEW_LINE || token->type == EOF_TYPE){
            return SEMANTIC_ERROR_CHYBEJICI_PREBYVAJICI_VYRAZ_V_PRIKAZU_NAVRATU_Z_FUNKCE;
        }
    }

    if(current_function->return_type == Void && token->type != NEW_LINE){
        return SEMANTIC_ERROR_CHYBEJICI_PREBYVAJICI_VYRAZ_V_PRIKAZU_NAVRATU_Z_FUNKCE;
    }
    if(current_function->return_type != Void){
    if(token->type != NEW_LINE || token->type != EOF_TYPE){
        token_t *tokentopass = (token_t*)malloc(sizeof(token_t));
        tokentopass->type = UNKNOWN;
        error = parser_expression(scanner, token, &type_of_variable, &if_while_condition, false, &tokentopass);
        if(error != SUCCESS){
            if(current_function->return_type == Void && error == SEMANTIC_ERROR_CHYBEJICI_PREBYVAJICI_VYRAZ_V_PRIKAZU_NAVRATU_Z_FUNKCE){
            }else{
                return error;
            }
            return error;
        }
    }
    }
    
    // if(current_function->return_type == Void && error == SEMANTIC_ERROR_OTHERS){
    //     return SEMANTIC_ERROR_CHYBEJICI_PREBYVAJICI_VYRAZ_V_PRIKAZU_NAVRATU_Z_FUNKCE;
    // }

    //check if type of return and type of function return type is same
    if(current_function->return_type != Void){
        if(type_of_variable != current_function->return_type){
            return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
        }
    }
    
    return SUCCESS;
}

// function call check
// function_id(parameters)
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
                                    return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
                                }
                                if(id->node_data_type == FUNCTION){
                                    //todo error
                                    return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
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
                            return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
                        }
                        if(id->node_data_type == FUNCTION){
                            //todo error
                            return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
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

    //add function_call
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

//check if parameters of function call are same as function parameters
error_t fun_calls_handler(){
    for(int i = 0; i < fun_calls_num; i++){
        bst_node *node = bst_search(stack->stack_array[0], fun_calls[i].id);
        if(node == NULL){
            return SEMANTIC_ERROR_UNDEF_FUN_OR_REDEF_VAR;
        }
        if(node->node_data_type != FUNCTION){
            return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
        }
        
        sym_t_function *node_fun = node->data;
        if(node_fun->num_params != fun_calls[i].num_params){
            return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
        }
        for(int j = 0; j < fun_calls[i].num_params; j++){
            if(fun_calls[i].params[j].param_name != NULL){
                if(strcmp(node_fun->params[j].param_name, fun_calls[i].params[j].param_name) != 0){
                    return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
                }
            }else {
                if(strcmp(node_fun->params[j].param_name, "_") != 0){
                    return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
                }
            }
            if(!check_type_compatibility(node_fun->params[j].param_type, fun_calls[i].params[j].param_type)){
                return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
            }
        }
        // //printf("return type call: %s, def: %s\n", variable_type_to_str(fun_calls[i].return_type), variable_type_to_str(node_fun->return_type));
        if(!check_type_compatibility(fun_calls[i].return_type, node_fun->return_type)){
            return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
        }
    }
    return SUCCESS;
}



void print_funcall(){
    //printf("Printing fun calls:\n");
    for(int i = 0; i < fun_calls_num; i++){
        //printf("%s(", fun_calls[i].id);
        for(int j = 0; j < fun_calls[i].num_params; j++){
            if(fun_calls[i].params[j].param_name != NULL){
                //printf("%s:", fun_calls[i].params[j].param_name);
            }

            //printf("%s   ", variable_type_to_str(fun_calls[i].params[j].param_type));

        }
        //printf(")\n");
    }

}


error_t parser_if_or_while_statement(scanner_t *scanner, token_t *token, bool if_or_while){
    error_t error;
    bool if_while = false;
    token_t *token_to_pass;
    token_to_pass = (token_t*)malloc(sizeof(token_t));
    token_to_pass->type = UNKNOWN;
    
    //new lines between if and statemet
    while(true){
        error = get_token(scanner, &token);
        if(error != SUCCESS){
            return error;
        }

        if(token->type == NEW_LINE){
            continue;
        }else if(token->type == LEFT_PAR){
            break;
        }else if(token->type == KEYWORD){
        //syntax let id
            if(strcmp(token->data, "let") == 0){
                break;
            }else{
                return SYNTAX_ERROR;
            }
        }else{
        //got first token from expression
            break;
        }
    }
    error = parser_expression(scanner, token, Not_specified, &if_while, true, &token_to_pass);
    if(error != SUCCESS){
        return error;
    }
    //there is no bool operator
    if(if_while == false){
        return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
    }


    //body {..}
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

        //else body {..}
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
    error = run_parser(scanner, token);
    if(error != SUCCESS){
        return error;
    }
     if(token->type == EOF_TYPE){
        return SYNTAX_ERROR;
    }
    return SUCCESS;
}


bool is_it_built_in_function(char *func_name){
    if( strcmp(func_name, "readString") == 0 ||
        strcmp(func_name, "readInt") == 0 ||
        strcmp(func_name, "readDouble") == 0 ||
        strcmp(func_name, "write") == 0 ||
        strcmp(func_name, "Int2Double") == 0 ||
        strcmp(func_name, "Double2Int") == 0 ||
        strcmp(func_name, "length") == 0 ||
        strcmp(func_name, "substring") == 0 ||
        strcmp(func_name, "ord") == 0 ||
        strcmp(func_name, "chr") == 0 ){
            
            return true;
        }else{
            return false;
        }
}

error_t parser_built_in_function(scanner_t *scanner, token_t *token, char *func_name, bst_node *variable){

    if(strcmp(func_name, "readString") == 0){
        return built_in_readString(scanner, token, variable);
    }else if(strcmp(func_name, "readInt") == 0){
        return built_in_readInt(scanner, token, variable);
    }else if(strcmp(func_name, "readDouble") == 0){
        return built_in_readDouble(scanner, token, variable);
    }else if(strcmp(func_name, "write") == 0){
        return built_in_write(scanner, token);
    }else if(strcmp(func_name, "Int2Double") == 0){
        return built_in_Int2Double(scanner, token, variable);
    }else if(strcmp(func_name, "Double2Int") == 0){
        return built_in_Double2Int(scanner, token, variable);
    }else if(strcmp(func_name, "length") == 0){
        return built_in_lenght(scanner, token, variable);
    }else if(strcmp(func_name, "substring") == 0){
        return built_in_substring(scanner, token, variable);
    }else if(strcmp(func_name, "ord") == 0){
        return built_in_ord(scanner, token, variable);
    }else if(strcmp(func_name, "chr") == 0){
        return built_in_chr(scanner, token, variable);
    } 
}

 error_t built_in_readString(scanner_t *scanner, token_t *token, bst_node *var){
    error_t error;

    if(var != NULL){
        if(var->variable_type != Not_specified){
            if(var->variable_type != String_nil){
                return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
            }else{
                insert_variable_data(var, "simulation");
            }
        }else{
            var->variable_type = String_nil;
            insert_variable_data(var, "simulation");
        }
    }

    error = get_token(scanner, &token);
    CHECKERROR(error);

    if(token->type == IDENTIFIER || token->type == STRING || token->type == INT || token->type == DOUBLE){
        return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
    }else if(token->type == RIGHT_PAR){
        return SUCCESS;
    }else{
        return SYNTAX_ERROR;
    }
 }

 error_t built_in_readInt(scanner_t *scanner, token_t *token, bst_node *var){
     error_t error;

    if(var != NULL){
        if(var->variable_type != Not_specified){
            if(var->variable_type != Int_nil){
                return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
            }else{
                insert_variable_data(var, "111");
            }
        }else{
            var->variable_type = Int_nil;
            insert_variable_data(var, "111");
        }
    }

    error = get_token(scanner, &token);
    CHECKERROR(error);

    if(token->type == IDENTIFIER || token->type == STRING || token->type == INT || token->type == DOUBLE){
        return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
    }else if(token->type == RIGHT_PAR){
        return SUCCESS;
    }else{
        return SYNTAX_ERROR;
    }
 }

 error_t built_in_readDouble(scanner_t *scanner, token_t *token, bst_node *var){
    error_t error;

    if(var != NULL){
        if(var->variable_type != Not_specified){
            if(var->variable_type != Double_nil){
                return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
            }else{
                insert_variable_data(var, "111.1");
            }
        }else{
            var->variable_type = Double_nil;
            insert_variable_data(var, "111.1");
        }
    }

    error = get_token(scanner, &token);
    CHECKERROR(error);

    if(token->type == IDENTIFIER || token->type == STRING || token->type == INT || token->type == DOUBLE){
        return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
    }else if(token->type == RIGHT_PAR){
        return SUCCESS;
    }else{
        return SYNTAX_ERROR;
    }
 }

 error_t built_in_write(scanner_t *scanner, token_t *token){
     error_t error;
     bst_node *variable;
     bool need_comma = false;

     while(true){
         error = get_token(scanner, &token);
         CHECKERROR(error);
         if(token->type == RIGHT_PAR){
            break;
         }else{
         if(need_comma == true){
             if(token->type == COMMA){
                need_comma = false;
                 continue;
             }else{
                 return SYNTAX_ERROR;
             }
         }else{
             if(token->type == INT){
                 int int_to_print = atoi(token->data);
                 //printf("%d", int_to_print);
                 need_comma = true;
             }else if(token->type == DOUBLE){
                 double double_to_print;
                 sscanf(token->data, "%lf", &double_to_print);
                 //printf("%a", double_to_print);
                 need_comma = true;
             }else if(token->type == STRING){
                 //printf("%s", token->data);
                 need_comma = true;
             }else if(token->type == KEYWORD){
                 if(strcmp(token->data, "nil") == 0){
                     //printf("");
                     need_comma = true;
                 }else{
                     return SYNTAX_ERROR;
                 }
             }else if(token->type == IDENTIFIER){
                need_comma = true;
                 variable = search_variable_in_all_scopes(stack, token->data);
                 if(variable == NULL){
                     return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
                 }
                 if(variable->data == NULL){
                     return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
                 }
                 if(variable->variable_type == Nil){
                     //printf("");
                     continue;
                 }
                 if(variable->variable_type == Int || variable->variable_type == Int_nil){
                     sym_t_variable *var = (sym_t_variable *)variable->data;
                     if(strcmp(var->data, "nil") == 0){
                         //printf("");
                     }else{
                         int int_to_print = atoi(var->data);
                         //printf("%d", int_to_print);
                     }
                 }else if(variable->variable_type == Double || variable->variable_type == Double_nil){
                     sym_t_variable *var = (sym_t_variable *)variable->data;
                     if(strcmp(var->data, "nil") == 0){
                         //printf("");
                     }else{
                         double double_to_print;
                         sscanf(var->data, "%lf", &double_to_print);
                         //printf("%a", double_to_print);
                     }
                 }else if(variable->variable_type == String || variable->variable_type == String_nil){
                     sym_t_variable *var = (sym_t_variable *)variable->data;
                     if(strcmp(var->data, "nil") == 0){
                         //printf("");
                     }else{
                         //printf("%s", var->data);
                     }
                 }
             }else{
                 return SYNTAX_ERROR;
             }
         }
     } 
     }
     return SUCCESS;
 }

 error_t built_in_Int2Double(scanner_t *scanner, token_t *token, bst_node *var){
     error_t error;
     bst_node *param_variable;
    
     error = get_token(scanner, &token);
     CHECKERROR(error);

     if(token->type == INT){

     }else if(token->type == IDENTIFIER){
         param_variable = search_variable_in_all_scopes(stack, token->data);
         if(param_variable == NULL){
             return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
         }
         if(param_variable->data == NULL){
             return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
         }
         if(param_variable->variable_type != Int){
             return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
         }
     }else if (token->type == INT || token->type == DOUBLE){
        return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
    }else if(token->type == KEYWORD){
        if(strcmp(token->data, "nil") == 0){
            return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
        }else{
            return SYNTAX_ERROR;
        }
    }else{
        return SYNTAX_ERROR;
    }

    if(var != NULL){
        if(var->variable_type != Not_specified){
            if(var->variable_type != Double && var->variable_type != Double_nil){
                return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
            }else{
                insert_variable_data(var, ((sym_t_variable*)param_variable->data)->data);
            }
        }else{
            var->variable_type = Double;
            insert_variable_data(var, ((sym_t_variable*)param_variable->data)->data);
        }
    }

     error = get_token(scanner, &token);
     CHECKERROR(error);
     if(token->type == COMMA){
        error = get_token(scanner, &token);
        CHECKERROR(error);
        if(token->type == IDENTIFIER || token->type == STRING || token->type == INT || token->type == DOUBLE){
            return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;           
        }else if(token->type == KEYWORD){
            if(strcmp(token->data, "nil") == 0){
                return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
            }else{
                return SYNTAX_ERROR;
            }
        }else{
            return SYNTAX_ERROR;
       }
     }else if(token->type == RIGHT_PAR){
        return SUCCESS;
     }else{
        return SYNTAX_ERROR;
     }
 }

 error_t built_in_Double2Int(scanner_t *scanner, token_t *token, bst_node *var){
     error_t error;
     bst_node *param_variable;
    
     error = get_token(scanner, &token);
     CHECKERROR(error);

     if(token->type == DOUBLE){

     }else if(token->type == IDENTIFIER){
         param_variable = search_variable_in_all_scopes(stack, token->data);
         if(param_variable == NULL){
             return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
         }
         if(param_variable->data == NULL){
             return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
         }
         if(param_variable->variable_type != Double){
             return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
         }
     }else if (token->type == INT || token->type == DOUBLE){
        return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
    }else if(token->type == KEYWORD){
        if(strcmp(token->data, "nil") == 0){
            return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
        }else{
            return SYNTAX_ERROR;
        }
    }else{
        return SYNTAX_ERROR;
    }

    if(var != NULL){
        if(var->variable_type != Not_specified){
            if(var->variable_type != Int && var->variable_type != Int_nil){
                return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
            }else{
                insert_variable_data(var, ((sym_t_variable*)param_variable->data)->data);
            }
        }else{
            var->variable_type = Int;
            insert_variable_data(var, ((sym_t_variable*)param_variable->data)->data);
        }
    }

     error = get_token(scanner, &token);
     CHECKERROR(error);
     if(token->type == COMMA){
        error = get_token(scanner, &token);
        CHECKERROR(error);
        if(token->type == IDENTIFIER || token->type == STRING || token->type == INT || token->type == DOUBLE){
            return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;           
        }else if(token->type == KEYWORD){
            if(strcmp(token->data, "nil") == 0){
                return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
            }else{
                return SYNTAX_ERROR;
            }
        }else{
            return SYNTAX_ERROR;
       }
     }else if(token->type == RIGHT_PAR){
        return SUCCESS;
     }else{
        return SYNTAX_ERROR;
     }
 }

error_t built_in_lenght(scanner_t *scanner, token_t *token, bst_node *var){
    error_t error;
    bst_node *param_variable;

    if(var != NULL){
        if(var->variable_type != Not_specified){
            if(var->variable_type != Int && var->variable_type != Int_nil){
                return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
            }else{
                insert_variable_data(var, "111");
            }
        }else{
            var->variable_type = Int;
            insert_variable_data(var, "111");
        }
    }

    error = get_token(scanner, &token);
     CHECKERROR(error);
    
    if(token->type == STRING){

    }else if(token->type == IDENTIFIER){
        param_variable = search_variable_in_all_scopes(stack, token->data);
        if(param_variable == NULL){
            return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
        }
        if(param_variable->data == NULL){
            return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
        }
        if(param_variable->variable_type != String){
            return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
        }
    }else if (token->type == INT || token->type == DOUBLE){
        return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
    }else if(token->type == KEYWORD){
        if(strcmp(token->data, "nil") == 0){
            return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
        }else{
            return SYNTAX_ERROR;
        }
    }else{
        return SYNTAX_ERROR;
    }

    error = get_token(scanner, &token);
    CHECKERROR(error);
    if(token->type == COMMA){
       error = get_token(scanner, &token);
       CHECKERROR(error);
       if(token->type == IDENTIFIER || token->type == STRING || token->type == INT || token->type == DOUBLE){
           return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;           
       }else if(token->type == KEYWORD){
            if(strcmp(token->data, "nil") == 0){
                return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
            }else{
                return SYNTAX_ERROR;
            }
        }else{
            return SYNTAX_ERROR;
        }
    }else if(token->type == RIGHT_PAR){
       return SUCCESS;
    }else{
       return SYNTAX_ERROR;
    }
}

error_t check_param(scanner_t *scanner, token_t *token, char *name_of_param, token_type_t token_type1, token_type_t token_type2){
    error_t error;
    bst_node *param_variable;

    //name of parametr
    error = get_token(scanner, &token);
    CHECKERROR(error);
    if(token->data == NULL){
        return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
    }

    if(strcmp(token->data, name_of_param) != 0){
        return SYNTAX_ERROR;
    }
    
    //colon
    error = get_token(scanner, &token);
    CHECKERROR(error);
    if(token->type != COLON){
        return SYNTAX_ERROR;
    }

    //parametr
    error = get_token(scanner, &token);
    CHECKERROR(error);
    if(token->type == token_type1){

    }else if(token->type == IDENTIFIER){
        param_variable = search_variable_in_all_scopes(stack, token->data);
        if(param_variable == NULL){
            return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
        }
        if(param_variable->data == NULL){
            return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
        }
        if(param_variable->variable_type != String){
            return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
        }
    }else if (token->type == token_type2 || token->type == DOUBLE){
        return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
    }else if(token->type == KEYWORD){
        if(strcmp(token->data, "nil") == 0){
            return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
        }else{
            return SYNTAX_ERROR;
        }
    }else{
        return SYNTAX_ERROR;
    }
    return SUCCESS;  
}

error_t built_in_substring(scanner_t *scanner, token_t *token, bst_node *var){
    error_t error;
    bst_node *param_variable;
    
    if(var != NULL){
        if(var->variable_type != Not_specified){
            if(var->variable_type != String_nil){
                return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
            }else{
                insert_variable_data(var, "simulation");
            }
        }else{
            var->variable_type = String_nil;
            insert_variable_data(var, "simulation");
        }
    }

    error = check_param(scanner, token, "of", STRING, INT);
    CHECKERROR(error);
    
    //comma
    error = get_token(scanner, &token);
    CHECKERROR(error);
    if(token->type == COMMA){
        
    }else if(token->type == RIGHT_PAR){
        return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
    }else{
        return SYNTAX_ERROR;
    }

    error = check_param(scanner, token, "startingAt", INT, STRING);
    CHECKERROR(error);
    
    //comma
    error = get_token(scanner, &token);
    CHECKERROR(error);
    if(token->type == COMMA){
        
    }else if(token->type == RIGHT_PAR){
        return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
    }else{
        return SYNTAX_ERROR;
    }

    error = check_param(scanner, token, "endingBefore", INT, STRING);
    CHECKERROR(error);
    
    error = get_token(scanner, &token);
     CHECKERROR(error);
     if(token->type == COMMA){
        error = get_token(scanner, &token);
        CHECKERROR(error);
        if(token->type == IDENTIFIER || token->type == STRING || token->type == INT || token->type == DOUBLE){
            return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;           
        }else if(token->type == KEYWORD){
            if(strcmp(token->data, "nil") == 0){
                return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
            }else{
                return SYNTAX_ERROR;
            }
        }else{
            return SYNTAX_ERROR;
       }
     }else if(token->type == RIGHT_PAR){
        return SUCCESS;
     }else{
        return SYNTAX_ERROR;
     }
}

error_t built_in_ord(scanner_t *scanner, token_t *token, bst_node *var){
    error_t error;
    bst_node *param_variable;
    
    if(var != NULL){
        if(var->variable_type != Not_specified){
            if(var->variable_type != Int && var->variable_type != Int_nil){
                return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
            }else{
                insert_variable_data(var, "111");
            }
        }else{
            var->variable_type = Int;
            insert_variable_data(var, "111");
        }
    }

    error = get_token(scanner, &token);
     CHECKERROR(error);
    
    if(token->type == STRING){

    }else if(token->type == IDENTIFIER){
        param_variable = search_variable_in_all_scopes(stack, token->data);
        if(param_variable == NULL){
            return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
        }
        if(param_variable->data == NULL){
            return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
        }
        if(param_variable->variable_type != String){
            return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
        }
    }else if (token->type == INT || token->type == DOUBLE){
        return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
    }else if(token->type == KEYWORD){
        if(strcmp(token->data, "nil") == 0){
            return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
        }else{
            return SYNTAX_ERROR;
        }
    }else{
        return SYNTAX_ERROR;
    }

    error = get_token(scanner, &token);
    CHECKERROR(error);
    if(token->type == COMMA){
       error = get_token(scanner, &token);
       CHECKERROR(error);
       if(token->type == IDENTIFIER || token->type == STRING || token->type == INT || token->type == DOUBLE){
           return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;           
       }else if(token->type == KEYWORD){
            if(strcmp(token->data, "nil") == 0){
                return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
            }else{
                return SYNTAX_ERROR;
            }
        }else{
            return SYNTAX_ERROR;
        }
    }else if(token->type == RIGHT_PAR){
       return SUCCESS;
    }else{
       return SYNTAX_ERROR;
    }
}

error_t built_in_chr(scanner_t *scanner, token_t *token, bst_node *var){
    error_t error;
    bst_node *param_variable;
    
    if(var != NULL){
        if(var->variable_type != Not_specified){
            if(var->variable_type != String_nil && var->variable_type != String){
                return SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL;
            }else{
                insert_variable_data(var, "simulation");
            }
        }else{
            var->variable_type = String;
            insert_variable_data(var, "simulation");
        }
    }

    error = get_token(scanner, &token);
     CHECKERROR(error);
    
    if(token->type == INT){

    }else if(token->type == IDENTIFIER){
        param_variable = search_variable_in_all_scopes(stack, token->data);
        if(param_variable == NULL){
            return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
        }
        if(param_variable->data == NULL){
            return SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT;
        }
        if(param_variable->variable_type != Int){
            return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
        }
    }else if (token->type == STRING || token->type == DOUBLE){
        return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
    }else if(token->type == KEYWORD){
        if(strcmp(token->data, "nil") == 0){
            return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
        }else{
            return SYNTAX_ERROR;
        }
    }else{
        return SYNTAX_ERROR;
    }

    error = get_token(scanner, &token);
    CHECKERROR(error);
    if(token->type == COMMA){
       error = get_token(scanner, &token);
       CHECKERROR(error);
       if(token->type == IDENTIFIER || token->type == STRING || token->type == INT || token->type == DOUBLE){
           return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;           
       }else if(token->type == KEYWORD){
            if(strcmp(token->data, "nil") == 0){
                return SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE;
            }else{
                return SYNTAX_ERROR;
            }
        }else{
            return SYNTAX_ERROR;
        }
    }else if(token->type == RIGHT_PAR){
       return SUCCESS;
    }else{
       return SYNTAX_ERROR;
    }
}

#endif
