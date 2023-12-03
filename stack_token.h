#include <stdbool.h>
#include <stdlib.h>

typedef struct stack_item stack_item_t;
struct stack_item{
  stack_item_t* next_item;
  int value_item;
}
typedef struct int_stack_item{
  stack_item_t* top;
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






