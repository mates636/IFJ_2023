#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "scanner.h"
#include "symtable.h"
#include "error_code.h"

#ifndef PARSER_H
#define PARSER_H

void init_parser();
void free_parser();

error_t run_parser(scanner_t *scanner);
error_t parser_analyse(scanner_t *scanner, token_t *token);

error_t parser_variable(scanner_t *scanner, token_t *t);
error_t parser_variable_identifier(scanner_t *scanner, token_t *token, bool can_modify);
error_t parser_variable_type_and_data(scanner_t *scanner, token_t *token, bst_node *tree_node);
//error_t parser_function();
//error_t parser_if_statement();
//error_t parser_while_statement();

#endif