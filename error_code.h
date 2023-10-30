#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum error_t{
    SUCCESS = 0,
    LEXICAL_ERROR = 1,
    SYNTAX_ERROR = 2,
    INTERN_ERROR = 99
}error_t;