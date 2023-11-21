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

error_t parser_expression(scanner_t *scanner, token_t *token, variable_type *control_type);
    bool push_or_compose(expression_s **expression_stack, int new_operator_priority);
    error_t expression_compose(expression_s **expression_stack, variable_type *expression_type);
    error_t parser_expression_nil_convert(token_t **left_token, token_t *right_token);
    error_t parser_expression_type_control_rel_operators(token_t *token, variable_type *type_control, token_type_t type_token);
    error_t parser_expression_type_control_arithmetic_strings(token_t *token, variable_type *type_control);
    error_t parser_expression_type_control_first_value(token_t *token, variable_type *type_control);

error_t parser_function(scanner_t *scanner, token_t *token);
error_t parser_argument(scanner_t *scanner, token_t *token, sym_t_function *struktura);
error_t parser_return_type(scanner_t *scanner, token_t *token, sym_t_function *struktura);

error_t parser_if(scanner_t *scanner, token_t *token);


//error_t parser_function();
//error_t parser_while_statement();

#endif
