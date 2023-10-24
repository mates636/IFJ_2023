#include "scanner.h"


scanner_t* init_scanner(FILE* f_input){
    scanner_t* scanner = (scanner_t*)malloc(sizeof(scanner_t));
    scanner->f_input = f_input;
    scanner->buffer_pos = 0;
    scanner->state = S_INIT;
    scanner->rewind = '\0';
    return scanner;
}

error_t get_token(scanner_t* scanner,token_t** token){
    while(1){
        char next_char;
        printf("state: %d \n", scanner->state);
        switch(scanner->state){
            case S_INIT:
                next_char = get_char(scanner);
                // printf("init got char %c\n", next_char);
                if(next_char == '\0'){
                    *token = init_token(EOF_TYPE);
                    return SUCCESS;
                }
                if(is_white(next_char)){
                    scanner->state = S_INIT;
                } else if(is_letter(next_char)){
                    add_char(next_char, scanner);
                    scanner->state = S_IDENTIFIERORKEYWORD;
                }else if(next_char == '_'){
                    add_char(next_char, scanner);
                    scanner->state = S_IDENTIFIERORKEYWORD;
                } else if(is_digit(next_char)){
                    add_char(next_char, scanner);
                    scanner->state = S_INT;
                } else if(next_char == '('){
                    *token =  init_token(LEFT_PAR);
                    return SUCCESS;
                } else if(next_char == ')'){
                    *token =  init_token(RIGHT_PAR);
                    return SUCCESS;
                } else if(next_char == '\"'){
                    scanner->state = S_STRING;
                } else {
                    return LEXICAL_ERROR;
                }
            break;
            case S_STRING:
                next_char = get_char(scanner);
                if(next_char == '\0'){
                    return LEXICAL_ERROR;
                } else if(next_char == '\n'){
                    return LEXICAL_ERROR;
                }
                else if(next_char != '\"'){
                    add_char(next_char, scanner);
                } else {
                    scanner->state = S_INIT;
                    *token = init_token_data(STRING, scanner->buffer, scanner->buffer_pos);
                    scanner->buffer_pos = 0;
                    return SUCCESS;
                }
            break;
            case S_INT:
                next_char = get_char(scanner);
                
                if(is_digit(next_char)){
                    add_char(next_char, scanner);
                } else {
                    scanner->state = S_INIT;
                    *token = init_token_data(INT, scanner->buffer, scanner->buffer_pos);
                    scanner->buffer_pos = 0;
                    scanner->rewind = next_char;
                    return SUCCESS;
                }
            break;
            case S_IDENTIFIERORKEYWORD:
                next_char = get_char(scanner);

                if(is_digit(next_char) || next_char == '_'){
                    add_char(next_char, scanner);
                    scanner->state = S_IDENTIFIER;
                }
                else if(is_letter(next_char)){
                    add_char(next_char, scanner);
                    scanner->state = S_IDENTIFIERORKEYWORD;
                }
                else{
                    if(strcmp(scanner->buffer, "_") == 0){
                        return LEXICAL_ERROR;
                    }
                    if(is_keyword(scanner)){
                        scanner->state = S_INIT;
                        *token = init_token_data(KEYWORD, scanner->buffer, scanner->buffer_pos);
                        scanner->rewind = next_char;
                        scanner->buffer_pos = 0;
                        return SUCCESS;
                    }else{
                        scanner->state = S_INIT;
                        *token = init_token_data(VARNAME, scanner->buffer, scanner->buffer_pos);
                        scanner->rewind = next_char;
                        scanner->buffer_pos = 0;
                        return SUCCESS;
                    }
                }
            break;

        }
    }

    return SUCCESS; 
}

token_t* init_token(token_type_t type){
    token_t* token = (token_t*)malloc(sizeof(token_t));
    token->type = type;
    token->data = NULL;
    return token;
}


token_t* init_token_data(token_type_t type, char* data, size_t data_len){
    token_t* token = (token_t*)malloc(sizeof(token_t));
    token->type = type;
    token->data = (char*)malloc(sizeof(char) * (data_len + 1));
    strncpy(token->data, data, data_len);
    token->data[data_len] = '\0';
    return token;
}

void destroy_token(token_t* token){
    free(token->data);
    free(token);
}

char get_char(scanner_t* scanner){
    char c;
    if(scanner->rewind != '\0'){
        char rew = scanner->rewind;
        scanner->rewind = '\0';
        return rew;
    }
    int n = fscanf(scanner->f_input, "%c", &c);
    if(n <= 0){
        return '\0';
    } else {
        return c;
    }
}

int is_white(char next_char){
    if(next_char == ' '){
        return 1;
    } else {
        return 0;
    }
}

int is_letter(char next_char){
    if((next_char >= 'a' && next_char <= 'z') || (next_char >= 'A' && next_char <= 'Z')){
        return 1;
    } else {
        return 0;
    }
}

int is_digit(char next_char){
    if(next_char >= '0' && next_char <= '9'){
        return 1;
    } else {
        return 0;
    }
}

bool is_keyword(scanner_t* scanner){
    char *keywords[] = {
        "Double", "else", "func", "if", "Int", "let", "nil", "return", "String", "var", "while"
    };

    int keywords_number = sizeof(keywords) / sizeof(keywords[0]);

    for(int i = 0; i < keywords_number; i++){
        if(strcmp(scanner->buffer, keywords[i]) == 0){
            return true;
        }
    }
    return false;
}

void add_char(char c, scanner_t* scanner){
    scanner->buffer[scanner->buffer_pos] = c;
    scanner->buffer_pos += 1;
}