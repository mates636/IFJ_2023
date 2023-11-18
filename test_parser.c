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
<<<<<<< HEAD
    error_t error = run_parser(scanner);
    printf("%d\n", error);
    return 0;
}
=======
    run_parser(scanner);
    free_parser();
    fclose(source);
    return 0;
}

#endif
>>>>>>> f870b633c4cc836d7be7edae4cf125bba5f68499
