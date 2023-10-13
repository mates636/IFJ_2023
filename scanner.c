#include "scanner.h"


scanner_t* init_scanner(FILE* f_input){
    scanner_t* scanner = (scanner_t*)malloc(sizeof(scanner_t));
    scanner->f_input = f_input;
    scanner->buffer_pos = 0;
    scanner->state = INIT;
    return scanner;
}

token_t* get_token(scanner_t* scanner){

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

