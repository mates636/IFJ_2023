#include <stdio.h>

#include "scanner.h"
#include "parser.h"
#include "error_code.h"
#include "symtable.h"


//#ifndef TEST_PARSER_C
#define TEST_PARSER_C


int main(int argc, char** argv){
    FILE* source = stdin; // stdin
    scanner_t* scanner = init_scanner(source);
    init_parser();
    error_t error = run_parser(scanner);
    free_parser();
    fclose(source);
    return error;
}