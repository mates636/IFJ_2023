#include <ctype.h>
#include "string_d.h"
#include "scanner.h"
#include "generator.h"
#define MAX_NUM 40

#define ADD_NEW(_addnew) 
  do { 
      if (!string_add_string(&code, (_addnew "\n"))) 
          return false; 
  } while (0)

#define ADD_CODE(_addcode) 
  do { 
      if (!string_add_string(&code, (_addcode))) 
          return false; 
  } while (0)

#define ADD_NUM(_addcode)
  do { 
      char string[MAX_NUM]; 
      sprintf(string, "%d", (_addcode)); 
      ADD_CODE(string); 
  } while (0)
static bool functions(){
  ADD_INST(LENGTH);
  ADD_INST(SUBSTR);
  ADD_INST(ASC);
  ADD_INST(CHR);
  return true;
}

static bool header(){
  ADD_INST("# Program started");
  ADD_INST(".IFJcode23");
  // DOPLNIT
  return true;
}

/**
* Inicialization of generator
*/
bool generator_start(){
  if((!string_init(&code)) || (!header()) || (!functions())){
    return false;
  }
  else{
    return false;
  }
}

/**
* Clear generator
*/
void generator_clear(){
  string_free(&code); 
}

/**
* Put generateed code to destination file then clear the generator
*/
void genrator_file(FILE *file){
  fputs(code.str, file);
  generator_clear();
}

/**
* Start of main scope, return true if successful
*/
bool genrator_main_start(){
  ADD_INST("\n# Start of main scope");
  // DOPLNIT
  return true;
}

/**
* End of main scope, return true if successful
*/
void genrator_main_end(){
  ADD_INST("\n# Start of main scope");
   // DOPLNIT
  return true;
}
