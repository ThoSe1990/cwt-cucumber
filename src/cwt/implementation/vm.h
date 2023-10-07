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
  cuke_value* slots;
} call_frame;

typedef enum {
  SKIPPED,
  PASSED,
  FAILED,
  UNDEFINED
} result_type;

typedef struct {
  result_type last;
  unsigned int passed;
  unsigned int failed; 
  unsigned int skipped;
  unsigned int undefined;
} results;


typedef struct {
  call_frame frames[FRAMES_MAX];
  int frame_count;
  results scenario_results;
  results step_results;
  cuke_value stack[STACK_MAX];
  cuke_value* stack_top;
  table steps; 
  table variables; 
  table strings; 
  obj*  objects;
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
void push(cuke_value cuke_value);
cuke_value pop();
void define_native(const char* name, cuke_step_t func);

#endif 