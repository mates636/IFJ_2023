#ifndef D_STRING_H
#define D_DTRING_H
#include <stdbool.h>

typedef struct{
  char *string;
  unsigned int length;
  unsigned allocated_size;
}D_string;

/**
* Inicialization of string
*/
bool string_init(D_string *str);

/**
* Clear the string
*/
void string_clear(D_string *str);

/**
* Free the alocated memory for string
*/
void string_free(D_string *str);

/**
* Add char
*/
bool string_add_ch(D_string *str, char ch);

/**
* Add constant
*/
int string_add_const(D_string *str, const char *const_str);



