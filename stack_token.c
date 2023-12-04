#include "stack_token.h"

/**
* Inicializace stacku
*/
int_stack_item_t* Init_stack(){
  int_stack_item_t* stack = malloc(sizeof(int_stack_item_t));
  stack->top_item = NULL;
  stack->count = 0;
  return stack;
}  

/**
* Uvolneni stacku
*/
void Free_stack(int_stack_item_t* stack){
  stack_item_t* item = stack->top_item;
  stack_item_t* next_item;
  while (item != NULL){
    next_item = item->next_item;
    free(item);
    item = next_item;
  }
}

/**
* Kontrola jestli je stack prazdny
*/
bool Empty_stack(int_stack_item_t* stack){
  return stack->top_item == NULL;
}

/**
* Pushne pointr
*/
void Push_stack(int_stack_item_t* stack, int* value){
  stack_item_t* item = malloc(sizeof(stack_item_t));
  item->value_item = value;
  item->next_item = stack->top_item;
  stack->top_item = item;
  stack->count++;
}

/**
* Popne item
*/
bool Pop_stack(int_stack_item_t* stack, int* value){
  if (Empty_stack(stack)){
    return false;
  }
  else{
    stack_item_t* item = stack->top_item;
    stack->top_item = item->next_item;
    *value = item->value_item;
    free(item);
    stack->count--;
    return true;
  }
}

/**
* Vrati top item ze stacku
*/
bool Top_stack(int_stack_item_t* stack, int* value){
  if (Empty_stack(stack)){
    return false;
  }
  else{
    *value = stack->top_item->value_item;
    return true;
  }
}

/**
* Vrati pocet
*/
bool Count_stack(int_stack_item_t* stack, int* value){
  if(!stack){
    return false;
  }
  else{
    *value = stack->count;
    return true;
  }
}

/**
* Vrati offset
*/
bool Offset_stack(int_stack_item_t* stack, int* value, int num){
  if(!stack){
      return false;
  }
  else if((stack->count - num >= stack->count) || (stack->count - num < 0)){
    return false;
  }
  else{
    stack_item_t* item = stack->top_item;
  }
  for(int i = stack->count - num; i > 0; i--){
    item = item->next_item;
  }
  *value = item->value;
  return true;
}
