#include <stdbool.h>
#include <stdlib.h>
#iclude "scanner.h"

typedef struct stack_item stack_item_t;
struct stack_item{
  stack_item_t* next_item;
  int value_item;
}
typedef struct int_stack_item{
  stack_item_t* top_item;
  int count;
}int_stack_item_t

/**
* Inicializace stacku
*/
int_stack_item_t* Init_stack();

/**
* Uvolneni stacku
*/
void Free_stack(int_stack_item_t* stack);

/**
* Kontrola jestli je stack prazdny
*/
bool Empty_stack(int_stack_item_t* stack);

/**
* Pushne pointr
*/
void Push_stack(int_stack_item_t* stack, int* value_item);

/**
* Popne item
*/
bool Pop_stack(int_stack_item_t* stack, int* value_item);

/**
* Vrati top item ze stacku
*/
bool Top_stack(int_stack_item_t* stack, int* value_item);

/**
* Vrati pocet
*/
bool Count_stack(int_stack_item_t* stack, int* value_item);

/**
* Vrati offset
*/
bool Offset_stack(int_stack_item_t* stack, int* value_item, int num);

typedef struct token_item_stack token_item_stack_t;
struct token_item_stack{
  token_item_stack_t* next_token;
  token_t value_token;
}
typedef struct token_stack{
  token_item_stack* top_token;
  FILE* file;
  int_stack_item_t* lx_stack;
}token_stack_t;

/**
* Inicializace stacku tokenu
*/
token_stack_t* Init_token_stack(FILE* file, int_stack_item_t* lx_stack);

/**
* Uvolneni stacku tokenu
*/
void Free_token_stack(token_stack_t* stack);

/**
* Kontrola jestli je stack tokenu prazdny
*/
bool Empty_token_stack(token_stack_t* stack);

/**
* Push pointr
*/
void Push_token_stack(token_stack_t* stack, token_t value_token);

/**
* Pop item ze stacku, pokud je stack prazdny bereme nasledujici token
*/
token_t Pop_token_stack(token_stack_t* stack);

/**
* Top item ze stacku, pokud je stack prazdny bereme nasledujici token
*/
token_t Top_token_stack(token_stack_t* stack);
