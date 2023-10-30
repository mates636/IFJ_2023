#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "error_code.h"

typedef enum token_type_t{
    UNKNOWN,
    VARNAME,
    SEMICOLON,
    LEFT_PAR,
    RIGHT_PAR,
    STRING,
    EOF_TYPE,
    INT,
    KEYWORD
}token_type_t;


typedef struct token_t
{
    token_type_t type;
    char* data;
}token_t;

typedef enum state_t{
    S_INIT,
    S_IDENTIFIER,
    S_EQUAL,
    S_LEFT_PAR,
    S_RIGHT_PAR,
    S_STRING,
    S_INT,

    //intermediate states
    S_IDENTIFIERORKEYWORD
}state_t;

typedef struct scanner_t{
    FILE* f_input;
    char buffer[10000];
    size_t buffer_pos;
    state_t state;
    char rewind;
}scanner_t;

error_t get_token(scanner_t* scanner,token_t** token);

token_t* init_token_data(token_type_t type, char* data, size_t data_len);
token_t* init_token(token_type_t type);

void destroy_token(token_t* token);
//void destroy_scanner()

scanner_t* init_scanner(FILE* f_input);

char get_char(scanner_t* scanner);

int is_white(char next_char);

int is_letter(char next_char);

int is_digit(char next_char);

bool is_keyword(scanner_t* scanner);

void add_char(char c, scanner_t* scanner);

token_t* init_token_data(token_type_t type, char* data, size_t data_len);
token_t* init_token(token_type_t type);

void destroy_token(token_t* token);
//void destroy_scanner()

scanner_t* init_scanner(FILE* f_input);

char get_char(scanner_t* scanner);

int is_white(char next_char);

int is_letter(char next_char);

int is_digit(char next_char);

void add_char(char c, scanner_t* scanner);