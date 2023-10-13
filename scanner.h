#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum token_type_t{
    UNKNOWN,
    VARNAME,
    SEMICOLON
}token_type_t;


typedef struct token_t
{
    token_type_t type;
    char* data;
} token_t;

typedef enum state_t{
    INIT,
    IDENTIFIER,
    EQUAL
}state_t;

typedef struct scanner_t{
    FILE* f_input;
    char buffer[10000];
    size_t buffer_pos;
    state_t state;
}scanner_t;


token_t* get_token(scanner_t* scanner);

token_t* init_token(token_type_t type, char* data, size_t data_len);

void destroy_token(token_t* token);

scanner_t* init_scanner(FILE* f_input);



