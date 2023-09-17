#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "debug.h"
#include "object.h"
#include "memory.h"
#include "vm.h"
#include "compiler.h"

vm g_vm;

static value clock_native(int arg_count, value* args)
{
  printf("clock native: %i -> %f\n",arg_count, AS_NUMBER(*args));
  return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
}

static void reset_stack()
{
  g_vm.stack_top = g_vm.stack;
  g_vm.frame_count = 0;
}

static void runtime_error(const char* format, ...)
{
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  for (int i = g_vm.frame_count - 1 ; i >= 0 ; i--)
  {
    call_frame* frame = &g_vm.frames[i];
    obj_function* func = frame->function;
    size_t instruction = frame->ip - func->chunk.code - 1;
    fprintf(stderr, "[line %d] in ", func->chunk.lines[instruction]);
    if (func->name == NULL)
    {
      fprintf(stderr, "script\n");
    } 
    else 
    {
      fprintf(stderr, "%s()\n", func->name->chars);
    }
  }

  reset_stack();
}

static void define_native(const char* name, native_func func)
{
  push(OBJ_VAL(copy_string(name, (int)strlen(name))));
  push(OBJ_VAL(new_native(func)));
  table_set(&g_vm.globals, AS_STRING(g_vm.stack[0]), g_vm.stack[1]);
  pop();
  pop();
}

void init_vm()
{
  reset_stack();
  g_vm.objects = NULL;
  init_table(&g_vm.globals);
  init_table(&g_vm.strings);

  define_native("clock", clock_native);
}
void free_vm()
{
  free_table(&g_vm.globals);
  free_table(&g_vm.strings);
  free_objects();
}

void push(value v)
{
  *g_vm.stack_top = v;
  g_vm.stack_top++;
}
value pop()
{
  g_vm.stack_top--;
  return *g_vm.stack_top;
}

static value peek(int distance)
{
  return g_vm.stack_top[-1 - distance];
}

static bool call(obj_function* func, int arg_count)
{
  if (arg_count != func->arity)
  {
    runtime_error("Expected %d arguments but got %d.", func->arity, arg_count);
    return false;
  }
  if (g_vm.frame_count == FRAMES_MAX)
  {
    runtime_error("Stack overflow.");
    return false;
  } 

  call_frame* frame = &g_vm.frames[g_vm.frame_count++];
  frame->function = func; 
  frame->ip = func->chunk.code;
  frame->slots = g_vm.stack_top - arg_count - 1;
  return true;
}

static bool call_value(value callee, int arg_count)
{
  if (IS_OBJ(callee))
  {
    switch(OBJ_TYPE(callee))
    {
      case OBJ_FUNCTION:
      {
        return call(AS_FUNCTION(callee), arg_count);
      } 
      case OBJ_NATIVE:
      {
        native_func native = AS_NATIVE(callee);
        value result = native(arg_count, g_vm.stack_top - arg_count);
        g_vm.stack_top -= arg_count+1;
        push(result);
        return true;
      }
      default: break; // non callable obj type
    }
  }
  runtime_error("Can only call functions and classes.");
  return false;
}

static bool is_falsey(value v)
{
  return IS_NIL(v) || (IS_BOOL(v) && !AS_BOOL(v)); 
}

static void concatenate()
{
  obj_string* b = AS_STRING(pop());
  obj_string* a = AS_STRING(pop());
  int length = a->length + b->length;
  char* chars = ALLOCATE(char, length+1);
  memcpy(chars, a->chars, a->length);
  memcpy(chars + a->length, b->chars, b->length);
  chars[length] = '\0';

  obj_string* result = take_string(chars, length);
  push(OBJ_VAL(result));
}

static interpret_result run() 
{
  call_frame* frame = &g_vm.frames[g_vm.frame_count-1];
#define READ_BYTE() (*frame->ip++)
#define READ_CONSTANT() (frame->function->chunk.constants.values[READ_BYTE()])
#define READ_STRING() AS_STRING(READ_CONSTANT())

  for(;;)
  {
  #ifdef DEBUG_TRACE_EXTENSION
    printf("        ");
    for (value* slot = g_vm.stack; slot < g_vm.stack_top; slot++)
    {
      printf("[ ");
      print_value(*slot);
      printf(" ]");
    }
    printf("\n");
    disassemble_instruction(&frame->function->chunk, (int)(frame->ip - frame->function->chunk.code));
  #endif 

    uint8_t instruction; 
    switch (instruction = READ_BYTE()) 
    {
      case OP_CONSTANT:
      {
        printf("OP_CONSTANT\n");
        // value constant = READ_CONSTANT();
        // push(constant);
      }
      break; case OP_NIL: 
      { 
        // obj_string* str = READ_STRING();
        printf("OP_NIL\n"); 
      }
      break; case OP_FEATURE: 
      { 
        printf("OP_FEATURE:\n"); 
      }
      break; case OP_SCENARIO: 
      { 
        printf("OP_SCENARIO\n"); 
      }
      break; case OP_NAME: 
      { 
        obj_string* str = READ_STRING();
        printf("OP_NAME %s \n", str->chars); 
      }
      break; case OP_DESCRIPTION: 
      { 
        obj_string* str = READ_STRING();
        printf("OP_DESCRIPTION %s \n", str->chars); 
      }
      break; case OP_STEP:
      {
        obj_string* str = READ_STRING();
        printf("OP_STEP %s \n", str->chars); 

        // value constant = READ_CONSTANT();
        // push(constant);
      }
      break; case OP_RETURN: 
      {
        printf("OP_RETURN\n");
        return INTERPRET_OK;
        // value result = pop();
        // g_vm.frame_count--;
        // if (g_vm.frame_count == 0) 
        // {
        //   pop();  
        //   return INTERPRET_OK;
        // }
        // else 
        // {
        //   g_vm.stack_top = frame->slots;
        //   push(result);
        //   frame = &g_vm.frames[g_vm.frame_count-1];
        //   break;
        // }
      }
      break; default: 
      {

      }
    }
  }
#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_STRING
}

interpret_result interpret(const char* source)
{
  obj_function* func = compile(source);
  if (func == NULL) 
  {
    return INTERPRET_COMPILE_ERROR;
  }

  push(OBJ_VAL(func));
  call(func, 0);

  return run();
}

