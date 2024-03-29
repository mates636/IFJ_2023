/******************************************************************************
 *                                  IFJ23
 *                                symtable.h
 *
 *                  Authors: Mikuláš Uřídil(xuridi01)
 *           Purpose: Source file with implementation of symtable
 *
 *                      Last change: 6.12.2023
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef ERROR_CODE
#define ERROR_CODE

typedef enum error_t
{
    SUCCESS = 0,
    LEXICAL_ERROR = 1,
    SYNTAX_ERROR = 2,
    SEMANTIC_ERROR_UNDEF_FUN_OR_REDEF_VAR = 3,
    SEMANTIC_ERROR_SPATNY_POCET_TYP_PARAMETRU_U_VOLANI_FUNKCE_OR_SPATNY_TYP_NAVRATOVE_HODNOTY_Z_FUNKCE = 4,
    SEMANTIC_ERROR_UNDEF_VAR_OR_NOT_INIT = 5,
    SEMANTIC_ERROR_CHYBEJICI_PREBYVAJICI_VYRAZ_V_PRIKAZU_NAVRATU_Z_FUNKCE = 6,
    SEMANTIC_ERROR_TYPE_COMP_AR_STR_REL = 7,
    SEMANTIC_ERROR_TYPE_CANNOT_INFERRED = 8,
    SEMANTIC_ERROR_OTHERS = 9,
    INTERN_ERROR = 99
} error_t;

#endif