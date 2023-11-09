#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef ERROR_CODE
#define ERROR_CODE

typedef enum error_t{
    SUCCESS = 0,
    LEXICAL_ERROR = 1,
    SYNTAX_ERROR = 2,
    SEMANTIC_ERROR_UNDEF_FUN_OR_REDEF_VAR = 3,
    INTERN_ERROR = 99
}error_t;

#endif 