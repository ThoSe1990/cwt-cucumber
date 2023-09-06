#include <stdio.h>

#include "common.h"
#include "debug.h"
#include "vm.h"
#include "compiler.h"

vm g_vm;

static void reset_stack()
{
  g_vm.stack_top = g_vm.stack;
}

void init_vm()
{
  reset_stack();
}
void free_vm()
{

}

static interpret_result run() 
{
#define READ_BYTE() (*g_vm.ip++)
#define READ_CONSTANT() (g_vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op) \
  do { \
    double b = pop(); \
    double a = pop(); \
    push(a op b); \
  } while (false)

  // for(;;)
  // {
#ifdef DEBUG_TRACE_EXTENSION
  printf("        ");
  for (value_t* slot = g_vm.stack; slot < g_vm.stack_top; slot++)
  {
    printf("[ ");
    print_value(*slot);
    printf(" ]");
  }
  printf("\n");
  disassemble_instruction(g_vm.chunk, (int)(g_vm.ip - g_vm.chunk->code));
#endif 

    // uint8_t instruction; 
    // switch (instruction = READ_BYTE()) 
    // {
    //   case OP_CONSTANT:
    //   {
    //     value_t constant = READ_CONSTANT();
    //     push(constant);
    //   }
    //   break; case OP_ADD:      BINARY_OP(+);
    //   break; case OP_SUBTRACT: BINARY_OP(-);
    //   break; case OP_MULTIPLY: BINARY_OP(*);
    //   break; case OP_DIVIDE:   BINARY_OP(/);
    //   break; case OP_NEGATE: push(-pop());
    //   break; case OP_RETURN: 
    //   {
    //     print_value(pop());
    //     printf("\n");
    //     return INTERPRET_OK;
    //   }
    // }
  // }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

interpret_result interpret(const char* source)
{
  chunk c;
  init_chunk(&c);

  if(!compile(source, &c))
  {
    free_chunk(&c);
    return INTERPRET_COMPILE_ERROR;
  }

  // g_vm.chunk = &c;
  // g_vm.ip = g_vm.chunk->code;

  // interpret_result result = run();

  free_chunk(&c);
  // return result; 
  return INTERPRET_OK;
}

void push(value_t value)
{
  *g_vm.stack_top = value;
  g_vm.stack_top++;
}
value_t pop()
{
  g_vm.stack_top--;
  return *g_vm.stack_top;
}
