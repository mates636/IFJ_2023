/******************************************************************************
 *                                  IFJ23
 *                                scanner.h
 *
 *                  Authors: Martin Priessnitz (xpries01)
 *           Purpose: Source file with implementation of lexer (scanner)
 *
 *                      Last change: 12.11.2023
 *****************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error_code.h"

#ifndef SCANNER_H
#define SCANNER_H

typedef enum token_type_t
{
    UNKNOWN,
    IDENTIFIER,
    LET_IDENTIFIER,
    FUNC_IDENTRIFIER, 
    NIL,              
    STRING_NIL,
    INT_NIL,
    DOUBLE_NIL,
    SEMICOLON, //;
    LEFT_PAR,  //(
    RIGHT_PAR, //)
    STRING,    //
    EOF_TYPE,  //
    INT,       //
    DOUBLE,    //
    KEYWORD,   //
    COLON,     //:
    RIGHT_BR,  //{
    LEFT_BR,   //}
    EQUALS,    //==
    ASSIGMENT, //=
    COMMA,     //,
    PLUS,      //+
    TWO_QUESTIONNAIRE,
    MINUS,       //-
    DIVIDE,      ///
    MULTIPLY,    //*
    LESS,        //<
    NOT_EQUALS,  //!=
    LESS_EQUALS, //<=
    MORE,        //>
    MORE_EQUALS, //>=
    RETURN_TYPE, //->
    COMMENT,     // //
    MULTILINE,   // /**/
    NIL_CONVERT, //??
    EXCLAMATION, //!
    NEW_LINE,    //\n
    NO_TYPE      //_
} token_type_t;

typedef struct token_t
{
    token_type_t type;
    char *data;
} token_t;

typedef enum state_t
{
    S_INIT,
    S_IDENTIFIER,
    S_EQUAL,
    S_LEFT_PAR,
    S_RIGHT_PAR,
    S_STRING,
    S_INT,
    S_DOUBLE_POINT,
    S_DOUBLE,
    S_EXPONENT,
    S_LESS,
    S_MORE,
    S_EQUALS,
    S_NOTEQUALS,
    S_RETURN_TYPE,
    S_STRING_ESCAPE,
    S_COMMENT,
    S_COMMENT2,
    S_COMMENT3,
    S_NIL_CONVERT,
    S_IDENTIFIERORKEYWORD,
    S_MULTI_STRING,
    S_MULTI_STRING2
} state_t;

typedef struct scanner_t
{
    FILE *f_input;
    char buffer[10000];
    size_t buffer_pos;
    state_t state;
    char rewind;
    char prev_char;
    char cur_char;
    int comm;
} scanner_t;

error_t get_token(scanner_t *scanner, token_t **token);

token_t *init_token_data(token_type_t type, char *data, size_t data_len);
token_t *init_token(token_type_t type);

void destroy_token(token_t *token);

scanner_t *init_scanner(FILE *f_input);

char get_char(scanner_t *scanner);

int is_white(char next_char);

int is_letter(char next_char);

int is_digit(char next_char);

int is_escape(char next_char);

bool is_keyword(scanner_t *scanner);

void add_char(char c, scanner_t *scanner);

token_t *init_token_data(token_type_t type, char *data, size_t data_len);
token_t *init_token(token_type_t type);

void destroy_token(token_t *token);

scanner_t *init_scanner(FILE *f_input);

char get_char(scanner_t *scanner);

int is_white(char next_char);

int is_letter(char next_char);

int is_digit(char next_char);

void add_char(char c, scanner_t *scanner);

#endif