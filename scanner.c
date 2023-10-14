#include "scanner.h"


scanner_t* init_scanner(FILE* f_input){
    scanner_t* scanner = (scanner_t*)malloc(sizeof(scanner_t));
    scanner->f_input = f_input;
    scanner->buffer_pos = 0;
    scanner->state = S_INIT;
    return scanner;
}

error_t get_token(scanner_t* scanner,token_t** token){
    while(1){
        char next_char;
        // printf("state: %d \n", scanner->state);
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
                    scanner->buffer[scanner->buffer_pos] = next_char;
                    scanner->buffer_pos += 1;
                } else if(is_digit(next_char)){
                    scanner->buffer[scanner->buffer_pos] = next_char;
                    scanner->buffer_pos = +1;
                } else if(next_char == '('){
                    scanner->state = S_LEFT_PAR;
                } else if(next_char == '\"'){
                    scanner->state = S_STRING;
                } else {
                    return LEXICAL_ERROR;
                }
            break;
            case S_LEFT_PAR:
                scanner->state = S_INIT;
                *token =  init_token(LEFT_PAR);
                return SUCCESS;
            
            break;
            case S_STRING:
                next_char = get_char(scanner);
                if(next_char == '\0'){
                    return LEXICAL_ERROR;
                }
                else if(next_char != '\"'){
                    scanner->buffer[scanner->buffer_pos++] = next_char;
                } else {
                    scanner->state = S_INIT;
                    *token = init_token_data(STRING, scanner->buffer, scanner->buffer_pos);
                    scanner->buffer_pos = 0;
                    return SUCCESS;
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