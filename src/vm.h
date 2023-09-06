#ifndef cwt_cucumber_vm_h
#define cwt_cucumber_vm_h

#include "chunk.h"

#define STACK_MAX 256

typedef struct {
  chunk* chunk;
  uint8_t* ip;
  value_t stack[STACK_MAX];
  value_t* stack_top;
} vm;

typedef enum {
  INTERPRET_OK, 
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} interpret_result;

void init_vm();
void free_vm();
interpret_result interpret(const char* source);
void push(value_t value);
value_t pop();

#endif 