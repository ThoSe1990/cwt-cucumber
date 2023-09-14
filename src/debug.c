#include <stdio.h>

#include "debug.h"
#include "value.h"

static int constant_instruction(const char* name, chunk* c, int offset)
{
  uint8_t constant = c->code[offset+1];
  printf("%-16s %4d '", name, constant);
  print_value(c->constants.values[constant]);
  printf("'\n");
  return offset+2;
}

static int simple_instruction(const char* name, int offset)
{
  printf("%s\n", name);
  return offset+1;
}

void disassemble_chunk(chunk* c, const char* name)
{
  printf("== %s ==\n", name);
  for (int offset = 0; offset < c->count;)
  {
    offset = disassemble_instruction(c, offset);
  }
}


int disassemble_instruction(chunk* c, int offset)
{
  printf("%04d ", offset);

  if (offset > 0 && c->lines[offset] == c->lines[offset-1])
  {
    printf("   | ");
  }
  else 
  {
    printf("%4d ", c->lines[offset]);
  }

  uint8_t instruction = c->code[offset];
  switch (instruction)
  {
  case OP_CONSTANT:
    return constant_instruction("OP_CONSTANT", c, offset);
  case OP_NIL:
    return simple_instruction("OP_NIL", offset);
  case OP_RETURN:
    return simple_instruction("OP_RETURN", offset);
  default:
    printf("Unknown opcode %d\n", instruction);
    return offset+1;
  }
}

