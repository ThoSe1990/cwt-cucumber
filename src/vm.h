#ifndef cwt_cucumber_vm_h
#define cwt_cucumber_vm_h

#include "object.h"
#include "chunk.h"
#include "table.h"
#include "value.h"

#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

typedef struct {
  obj_function* function;
  uint8_t* ip;
  value* slots;
} call_frame;

typedef struct {
  call_frame frames[FRAMES_MAX];
  int frame_count;
  value stack[STACK_MAX];
  value* stack_top;
  table steps;
  table globals;
  table strings;
  obj* objects;
} vm;

typedef enum {
  INTERPRET_OK, 
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} interpret_result;

extern vm g_vm;

void init_vm();
void free_vm();
interpret_result interpret(const char* source, const char* filename);
void push(value value);
value pop();
void define_native(const char* name, native_func func);

#endif 