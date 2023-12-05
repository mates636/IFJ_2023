#include "generator.h"


// Generator kodu
int generate(char **string, char *code, bool flag){
  if(flag){
   *string = (char *)realloc(*string, sizeof(char) * (strlen(*string) + strlen(code) + ending_0));
        if(string == NULL){
            return INTERNAL_ERROR;            
        }
        for(int i = 0; code[i] != '\0'; i++){
          *string[strlen(*string) + i] = code[i];
        }
        *string[strlen(*string) + strlen(code)] = '\0';
    }
    else{
        printf("%s", code);
    }

    return 0;
}
