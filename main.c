/******************************************************************************
 *                                  IFJ23
 *                                scanner.c
 *
 *                  Authors: Martin Priessnitz (xpries01), Mikuláš Uřídil(xuridi01)
 *           Purpose: Source file with implementation of main file
 *
 *                      Last change: 6.12.2023
 *****************************************************************************/

#include <stdio.h>
#include "scanner.h"
#include "parser.h"
#include "error_code.h"
#include "symtable.h"

#ifndef TEST_PARSER_C
#define TEST_PARSER_C

int main(int argc, char **argv)
{
    FILE *source = stdin; // stdin
    scanner_t *scanner = init_scanner(source);
    init_parser();
    token_t *token;
    error_t error = run_parser(scanner, token);
    if (error != SUCCESS)
    {      
        return error;
    }
    error = free_parser();

    fclose(source);
    free(scanner);
    return error;
}
#endif