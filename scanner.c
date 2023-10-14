#include "scanner.h"


scanner_t* init_scanner(FILE* f_input){
    scanner_t* scanner = (scanner_t*)malloc(sizeof(scanner_t));
    scanner->f_input = f_input;
    scanner->buffer_pos = 0;
    scanner->state = INIT;
    return scanner;
}

error_t get_token(scanner_t* scanner,token_t** token){
    while(1){
        char next_char = get_char(scanner);
        switch(scanner->state){
            case INIT:
                if(is_white){
                    scanner->state = INIT;
                } else if(is_letter){
                    scanner->buffer[scanner->buffer_pos] = next_char;
                    scanner->buffer_pos = +1;
                } else if(is_digit){
                    scanner->buffer[scanner->buffer_pos] = next_char;
                    scanner->buffer_pos = +1;
                }
            break;
        }
    }

    return SUCCESS; 
}


token_t* init_token(token_type_t type, char* data, size_t data_len){
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
    if(n == 0){
        return '\0';
    } else {
        return c;
    }
}

int is_white(char next_char){
    if(next_char == " "){
        return 1;
    } else {
        return 0;
    }
}

int is_letter(char next_char){
    if((next_char >= "a" && next_char <= "z") || (next_char >= "A" && next_char <= "Z")){
        return 1;
    } else {
        return 0;
    }
}

int is_digit(char next_char){
    if(next_char >= "0" && next_char <= "9"){
        return 1;
    } else {
        return 0;
    }
}