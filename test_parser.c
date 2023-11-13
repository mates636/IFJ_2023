#include <stdio.h>

#include "scanner.h"
#include "parser.h"
#include "error_code.h"
#include "symtable.h"


#ifndef TEST_PARSER_C
#define TEST_PARSER_C


int main(int argc, char** argv){
    FILE* source = fopen(argv[1], "r");
    scanner_t* scanner = init_scanner(source);
    init_parser();
    run_parser(scanner);
    free_parser();
    fclose(source);
    return 0;
}

#endif