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
#include "step_matcher.h"

vm g_vm;

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

void define_native(const char* name, cuke_step_t func)
{
  push(OBJ_VAL(copy_string(name, (int)strlen(name))));
  push(OBJ_VAL(new_native(func)));
  table_set_step(&g_vm.steps, AS_STRING(g_vm.stack[0]), g_vm.stack[1]);
  pop();
  pop();
}

void init_vm()
{
  reset_stack();
  g_vm.objects = NULL;
  init_table(&g_vm.variables);
  init_table(&g_vm.strings);
  init_table(&g_vm.steps);
}
void free_vm()
{
  free_table(&g_vm.variables);
  free_table(&g_vm.strings);
  free_table(&g_vm.steps);
  free_objects();
}

void push(cuke_value value)
{
  *g_vm.stack_top = value;
  g_vm.stack_top++;
}
cuke_value pop()
{
  g_vm.stack_top--;
  return *g_vm.stack_top;
}

static void replace(int position, cuke_value value)
{
  g_vm.stack[position] = value;
}

static cuke_value peek(int distance)
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

static bool call_value(cuke_value callee, int arg_count)
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
        cuke_step_t native = AS_NATIVE(callee);
        native(arg_count, g_vm.stack_top - arg_count);
        g_vm.stack_top -= arg_count+1;
        return true;
      }
      default: break; // non callable obj type
    }
  }
  runtime_error("Can only call functions and classes.");
  return false;
}

static bool is_falsey(cuke_value value)
{
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value)); 
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
#define READ_SHORT() \
  (frame->ip += 2, (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))
#define READ_STRING() AS_STRING(READ_CONSTANT())

  for(;;)
  {
  #ifdef DEBUG_TRACE_EXTENSION
    printf("        ");
    for (cuke_value* slot = g_vm.stack; slot < g_vm.stack_top; slot++)
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
        cuke_value constant = READ_CONSTANT();
        push(constant);
      }
      break; case OP_POP: pop(); 
      break; case OP_NIL: push(NIL_VAL);
      break; case OP_DEFINE_GLOBAL:
      {
        obj_string* name = READ_STRING();
        table_set(&g_vm.variables, name, peek(0));
        pop();
      }
      break; case OP_GET_GLOBAL:
      {
        obj_string* name = READ_STRING();
        cuke_value value; 
        if (!table_get(&g_vm.variables, name, &value))
        {
          runtime_error("Undefined variable '%s'.", name->chars);
          return INTERPRET_RUNTIME_ERROR;
        }
        else 
        {
          push(value);
        }
      }
      break; case OP_SET_GLOBAL:
      {
        obj_string* name = READ_STRING();
        if (table_set(&g_vm.variables, name, peek(0)))
        {
          table_delete(&g_vm.variables, name);
          runtime_error("Undefined variable '%s'.", name->chars);
          return INTERPRET_RUNTIME_ERROR;
        }
        pop();
      }
      break; case OP_JUMP_IF_FAILED:
      {
        uint16_t offset = READ_SHORT();
        if (g_vm.last_result == STEP_FAILED || g_vm.last_result == STEP_SKIPPED) 
        {
          g_vm.last_result = STEP_SKIPPED;
          frame->ip += offset;
        }
      }
      break; case OP_PRINT_LINE: 
      { 
        obj_string* name = READ_STRING();
        printf("[-----------] %s:\n", name->chars);
      }
      break; case OP_PRINT_LINEBREAK:
      {
        printf("[-----------]\n");
      }
      break; case OP_PRINT_RESULT:
      {
        obj_string* step = READ_STRING();
        switch (g_vm.last_result)
        {
          case STEP_PASSED: printf("\x1b[32m[  PASSED   ] %s\x1b[0m\n", step->chars);
          break; case STEP_FAILED: printf("\x1b[31m[  FAILED   ] %s\x1b[0m\n", step->chars);
          break; case STEP_SKIPPED: printf("\x1b[33m[  SKIPPED  ] %s\x1b[0m\n", step->chars);
          break; default: ;// shouldn't happen ... 
        }        
      }
      break; case OP_CALL_STEP:
      {
        obj_string* step_in_feature = READ_STRING();
        
        obj_string step_definition;
        cuke_value value;
        
        if (table_get_step(&g_vm.steps, step_in_feature, &value, &step_definition))
        {
          value_array args;
          init_value_array(&args); 

          parse_step(step_definition.chars, step_in_feature->chars, &args);
          
          // TODO check arg count properly? 
          for(int i = args.count-1 ; i >= 0 ; i--)
          {
            if (IS_NIL(args.values[i]))
            {
              args.values[i] = pop();
            }
          }
          
          cuke_step_t native = AS_NATIVE(value);
          native(args.count, args.values);

          free_value_array(&args);
        }
      }
      break; case OP_CALL:
      {
        g_vm.last_result = STEP_PASSED;
        int arg_count = READ_BYTE();
        if (!call_value(peek(arg_count), arg_count)) 
        {
          return INTERPRET_RUNTIME_ERROR;
        }
        frame = &g_vm.frames[g_vm.frame_count-1];
      } 
      break; case OP_RETURN: 
      {
        g_vm.frame_count--;
        if (g_vm.frame_count == 0) 
        {
          pop();  
          return INTERPRET_OK;
        }
        else 
        {
          g_vm.stack_top = frame->slots;
          frame = &g_vm.frames[g_vm.frame_count-1];
          break;
        }
      }
    }
  }
#undef READ_BYTE
#undef READ_SHORT
#undef READ_CONSTANT
#undef READ_STRING
}

interpret_result interpret(const char* source, const char* filename)
{
  obj_function* func = compile(source, filename);
  if (func == NULL) 
  {
    return INTERPRET_COMPILE_ERROR;
  }

  push(OBJ_VAL(func));
  call(func, 0);

  return run();
}

