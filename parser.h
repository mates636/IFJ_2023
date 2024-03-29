/******************************************************************************
 *                                  IFJ23
 *                                parser.h
 *
 *                  Authors: Mikuláš Uřídil(xuridi01), Martin Priessnitz (xpries01)
 *           Purpose: Source file with implementation of parser
 *
 *                      Last change: 6.12.2023
 *****************************************************************************/
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
error_t free_parser();

error_t run_parser(scanner_t *scanner, token_t *token);
error_t parser_analyse(scanner_t *scanner, token_t *token);

error_t parser_variable(scanner_t *scanner, token_t *t);
error_t parser_variable_identifier(scanner_t *scanner, token_t *token, bool can_modify);
error_t parser_variable_type_and_data(scanner_t *scanner, token_t *token, bst_node *tree_node);
error_t parser_id_assignment_function(scanner_t *scanner, token_t *token, token_t *function_id, bst_node *variable);

error_t parser_def_or_dec_variable(scanner_t *scanner, token_t *token, char *var_name);

error_t parser_built_in_function(scanner_t *scanner, token_t *token, char *func_name, bst_node *variable);
bool is_it_built_in_function(char *func_name);
error_t built_in_chr(scanner_t *scanner, token_t *token, bst_node *var);
error_t built_in_Double2Int(scanner_t *scanner, token_t *token, bst_node *var);
error_t built_in_Int2Double(scanner_t *scanner, token_t *token, bst_node *var);
error_t built_in_lenght(scanner_t *scanner, token_t *token, bst_node *var);
error_t built_in_ord(scanner_t *scanner, token_t *token, bst_node *var);
error_t built_in_readDouble(scanner_t *scanner, token_t *token, bst_node *var);
error_t built_in_readInt(scanner_t *scanner, token_t *token, bst_node *var);
error_t built_in_readString(scanner_t *scanner, token_t *token, bst_node *var);
error_t built_in_substring(scanner_t *scanner, token_t *token, bst_node *var);
error_t built_in_write(scanner_t *scanner, token_t *token);

error_t parser_expression(scanner_t *scanner, token_t *token, variable_type *control_type, bool *if_while_condition, bool is_it_while_or_if, token_t **token_to_pass);
bool push_or_compose(expression_s **expression_stack, int new_operator_priority);
error_t expression_compose(expression_s **expression_stack, variable_type *expression_type);
error_t parser_expression_nil_convert(token_t **left_token, token_t *right_token, variable_type *expression_type);
error_t parser_expression_type_control_rel_operators(token_t *token, variable_type *type_control, token_type_t type_token);
error_t parser_expression_type_control_arithmetic_strings(token_t *token, variable_type *type_control);
error_t parser_expression_type_control_first_value(token_t *token, variable_type *type_control);

error_t parser_function(scanner_t *scanner, token_t *token);
error_t parser_argument(scanner_t *scanner, token_t *token, sym_t_function *struktura);
error_t parser_return_type(scanner_t *scanner, token_t *token, sym_t_function *struktura);
error_t parser_return(scanner_t *scanner, token_t *token);
error_t parser_function_call(scanner_t *scanner, char *func_name, variable_type required_return_type);
error_t fun_calls_handler();
void print_funcall();

error_t parser_if_or_while_statement(scanner_t *scanner, token_t *token, bool if_or_while);
error_t parser_if_or_while_body(scanner_t *scanner, token_t *token);
#endif
