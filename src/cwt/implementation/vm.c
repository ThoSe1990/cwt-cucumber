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
#include "prints.h"
#include "../cucumber.h"

vm g_vm;

result_t* get_step_result()
{
  return &g_vm.step_results;
}
result_t* get_scenario_result()
{
  return &g_vm.scenario_results;
}

failed_scenarios_t* get_failed_scenarios()
{
  return &g_vm.failed_scenarios;
}

static void reset_stack()
{
  g_vm.stack_top = g_vm.stack;
  g_vm.frame_count = 0;
}

static void runtime_error(const char* format, ...)
{  
  start_red_on_stderr();
  call_frame* frame = &g_vm.frames[g_vm.frame_count - 1];
  obj_function* func = frame->function;
  size_t instruction = frame->ip - func->chunk.code - 1;
  fprintf(stderr, "[line %d] in ", func->chunk.lines[instruction]-1);  
  
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);
  end_red_on_stderr();
}

static void init_results(result_t* result)
{
  result->failed = 0;
  result->passed = 0;
  result->skipped = 0;
  result->undefined = 0;
  result->current = PASSED;
}

void define_step(const char* name, cuke_step_t func)
{
  push(OBJ_VAL(copy_string(name, (int)strlen(name))));
  push(OBJ_VAL(new_native(func)));
  table_set_step(&g_vm.steps, AS_STRING(g_vm.stack[0]), g_vm.stack[1], true);
  pop();
  pop();
}
void define_hook(const char* name, cuke_step_t func, const char* tag_expression)
{
  push(OBJ_VAL(copy_string(name, (int)strlen(name))));
  push(OBJ_VAL(new_hook(func, tag_expression)));
  table_set_step(&g_vm.hooks, AS_STRING(g_vm.stack[0]), g_vm.stack[1], false);
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
  init_table(&g_vm.hooks);
  init_results(&g_vm.scenario_results);
  init_results(&g_vm.step_results);
  init_value_array(&g_vm.failed_scenarios.name);
  init_value_array(&g_vm.failed_scenarios.location);
}
void free_vm()
{
  free_table(&g_vm.variables);
  free_table(&g_vm.strings);
  free_table(&g_vm.steps);
  free_table(&g_vm.hooks);
  free_value_array(&g_vm.failed_scenarios.name);
  free_value_array(&g_vm.failed_scenarios.location);
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

static int get_test_result()
{
  if (g_vm.scenario_results.failed > 0 
    || g_vm.scenario_results.undefined > 0
    || g_vm.scenario_results.skipped > 0)
  {
    return CUKE_FAILED;
  }
  else 
  {
    return CUKE_SUCCESS;
  }
}

void set_failed_scenario()
{
  if (g_vm.scenario_results.current == PASSED)
  {
    return ; 
  }
  write_value_array(&g_vm.failed_scenarios.name, OBJ_VAL(g_vm.current_scenario.name));
  write_value_array(&g_vm.failed_scenarios.location, OBJ_VAL(g_vm.current_scenario.location));
}

void set_step_result()
{
  switch (g_vm.step_results.current)
  {
    case PASSED: 
    {
      g_vm.step_results.passed++;
    }
    break; case FAILED: 
    {
      g_vm.scenario_results.current = FAILED;
      g_vm.step_results.failed++;
    }
    break; case SKIPPED: 
    {
      g_vm.scenario_results.current = SKIPPED;
      g_vm.step_results.skipped++;
    }
    break; case UNDEFINED: 
    {
      g_vm.scenario_results.current = g_vm.scenario_results.current == FAILED ? FAILED : UNDEFINED;
      g_vm.step_results.undefined++;
    }
  }
}
void print_step_result(obj_string* step)
{
  switch (g_vm.step_results.current)
  {
    case PASSED: print_green("[   PASSED    ] %s", step->chars);
    break; case FAILED: print_red("[   FAILED    ] %s", step->chars);
    break; case SKIPPED: print_blue("[   SKIPPED   ] %s", step->chars);
    break; case UNDEFINED: print_yellow("[  UNDEFINED  ] %s", step->chars);
  }  
}

static void print_line(const char* str, print_color color)
{
  switch(color)
  {
    case PRINT_WHITE: print_white(str);
    break; case PRINT_BLACK: print_black(str);
  }
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

static bool values_match(value_array* args)
{
  bool result = true;
  for(int i = args->count-1 ; i >= 0 ; i--)
  {
    cuke_value value = pop();
    if (!IS_INT(args->values[i]))
    {
      result = false;
    }
    else if (value.type != args->values[i].type)
    {
      result = false;
    }
    else 
    {
      args->values[i] = value;
    }
  }
  return result;
}

static bool call_value(cuke_value callee, int arg_count)
{
  if (IS_OBJ(callee))
  {
    switch(OBJ_TYPE(callee))
    {
      // TODO OBJ_FUNCTION does not exist in cucumber
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
    uint16_t instruction; 
    switch (instruction = READ_BYTE()) 
    {
      case OP_CONSTANT:
      {
        cuke_value constant = READ_CONSTANT();
        push(constant);
      }
      break; case OP_POP: pop(); 
      break; case OP_NIL: push(NIL_VAL);
      break; case OP_DEFINE_VARIABLE:
      {
        obj_string* name = READ_STRING();
        table_set(&g_vm.variables, name, peek(0));
      }
      break; case OP_GET_VARIABLE:
      {
        obj_string* name = READ_STRING();
        cuke_value value; 
        if (!table_get(&g_vm.variables, name, &value))
        {
          runtime_error("Undefined variable '%s'.", name->chars);
          g_vm.step_results.current = FAILED;
        }
        else 
        {
          push(value);
        }
      }
      break; case OP_SET_VARIABLE:
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
        if (g_vm.step_results.current != PASSED)
        {
          g_vm.step_results.current = SKIPPED;
          frame->ip += offset;
        }
      }
      break; case OP_HOOK:
      {
        int tag_count = READ_BYTE();
        obj_string* hook_str = AS_STRING(peek(tag_count));
        run_all_hooks(&g_vm.hooks, hook_str,  g_vm.stack_top-tag_count, tag_count);
    
        g_vm.stack_top -= tag_count+1;
        frame = &g_vm.frames[g_vm.frame_count-1];
      }
      break; case OP_CALL_STEP:
      {
        obj_string* step_in_feature = READ_STRING();
        obj_string step_definition;
        cuke_value value;      
        value_array args;
        init_value_array(&args); 
        if (table_get_step(&g_vm.steps, step_in_feature, &value, &step_definition)
          && parse_step(step_definition.chars, step_in_feature->chars, &args))
        {
          cuke_step_t native = AS_NATIVE(value);
          native(args.count, args.values);
        } 
        else 
        {
          g_vm.step_results.current = UNDEFINED;
          g_vm.scenario_results.current = FAILED;
        }
        free_value_array(&args);
      }
      break; case OP_CALL:
      {
        int arg_count = READ_BYTE();
        if (!call_value(peek(arg_count), arg_count)) 
        {
          return INTERPRET_RUNTIME_ERROR;
        }
        frame = &g_vm.frames[g_vm.frame_count-1];
      } 
      break; case OP_PRINT_LINE: 
      { 
        print_color color = READ_BYTE();
        obj_string* str = AS_STRING(pop());
        print_line(str->chars, color);
      }
      break; case OP_INIT_SCENARIO:
      {
        g_vm.current_scenario.name = AS_STRING(pop());
        g_vm.current_scenario.location = AS_STRING(pop());
      }
      break; case OP_PRINT_LINEBREAK:
      {
        printf("\n");
      }
      break; case OP_PRINT_SPACES:
      {
        printf("  ");
      }
      break; case OP_PRINT_STEP_RESULT:
      {
        obj_string* step = READ_STRING();
        print_step_result(step);
      }
      break; case OP_SET_STEP_RESULT:
      {
        set_step_result();
      }
      break; case OP_PRINT_BLACK:
      {
        obj_string* str = READ_STRING();
        print_black("  %s", str->chars);
      }
      break; case OP_SCENARIO_RESULT:
      {
        switch (g_vm.scenario_results.current)
        {
          case PASSED: g_vm.scenario_results.passed++;
          break; case FAILED: g_vm.scenario_results.failed++;
          break; case SKIPPED: g_vm.scenario_results.skipped++;
          break; case UNDEFINED: g_vm.scenario_results.undefined++;
        }
        set_failed_scenario();
        g_vm.scenario_results.current = PASSED;
        g_vm.step_results.current = PASSED;
      }
      break; case OP_RETURN: 
      {
        g_vm.frame_count--;
        if (g_vm.frame_count == 0) 
        {
          pop();  
          return get_test_result();
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

