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
    SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL = 7,
    SEMANTIC_ERROR_TYPE_CANNOT_INFERRED = 8,
    SEMANTIC_ERROR_OTHERS = 9,
    INTERN_ERROR = 99
}error_t;

#endif 