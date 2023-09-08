#include <stdio.h>

#include "debug.h"
#include "value.h"

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
  case 1:
    return simple_instruction("OP1", offset);
  case 2:
    return simple_instruction("OP2", offset);
  case 3:
    return simple_instruction("OP3", offset);
  default:
    printf("Unknown opcode %d\n", instruction);
    return offset+1;
  }
}

