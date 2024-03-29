#include "scanner.h"
#include <stdio.h>



int main(int argc, char** argv){
    FILE* source = fopen(argv[1], "r");
    scanner_t* scanner = init_scanner(source);
    while(1){
        token_t* token;
        error_t err = get_token(scanner, &token);
        if(err != SUCCESS){
            printf("chyba %d \n", err);
            break;
        }
        printf("token type: %d \n", token->type);
        if(token->data){
            printf("token data: \"%s\" \n", token->data);
        }
        if(token->type == EOF_TYPE){
            destroy_token(token);
            break;
        }
        
        destroy_token(token);
    }
    fclose(source);
    return 0;
}