#include "parser.h"

scope_stack *stack;

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
        error = get_token(scanner, &token);

    //TO DO 
    //SCANNER LEXEM ERRORS - nyni se nehlasi chybi ze scanneru 

        if(token->type == EOF_TYPE){
            destroy_token(token);
            break;
        }

        error = parser_analyse(scanner, token); 

        if(error != SUCCESS){
            printf("chyba %d \n", error);
            destroy_token(token);
            return error;
        }
        
        destroy_token(token);
    }
    return SUCCESS;
}

error_t parser_analyse(scanner_t *scanner, token_t *token){
    switch (token->type)
    {
    case KEYWORD:
        if(token->data == "if"){
            return parser_if_statement();
        }else if(token->data == "func"){
            return parser_function();
        }else if(token->data == "let"){
            return parser_variable(scanner, token);
        }else if(token->data == "var"){
            return parser_variable(scanner, token);
        }else if(token->data == "while"){
            return parser_while_statement();
        }else{
            return SYNTAX_ERROR;
        }
    default:
        return SYNTAX_ERROR;
    }
}

error_t parser_variable(scanner_t *scanner, token_t *t){
    bool can_modify;
    if(t->data == "let"){
        can_modify = false;
    }else{
        can_modify = true;
    }

    token_t *token;
    error_t error = get_token(scanner, &token);
    //if(token->type == )

}