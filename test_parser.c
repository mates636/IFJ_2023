#include "scanner.h"
#include "parser.h"
#include <stdio.h>


 
int main(int argc, char** argv){
    FILE* source = fopen(argv[1], "r");
    scanner_t* scanner = init_scanner(source);
    init_parser();
    error_t error = run_parser(scanner);
    printf("%d\n", error);
    return 0;
}