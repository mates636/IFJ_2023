#include "scanner.h"
#include "parser.h"
#include <stdio.h>


 
int main(int argc, char** argv){
    FILE* source = fopen(argv[1], "r");
    scanner_t* scanner = init_scanner(source);
    while(1){

    }
}