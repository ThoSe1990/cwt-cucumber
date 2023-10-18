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
static int jump_instruction(const char* name, int sign, chunk* c, int offset)
{
  uint16_t jump = (uint16_t)(c->code[offset+1] << 8);
  jump |= c->code[offset+2];
  printf("%-16s %4d -> %d\n", name, offset, offset+3+sign*jump);
  return offset + 3;
}
static int simple_instruction(const char* name, int offset)
{
  printf("%s\n", name);
  return offset+1;
}

static int byte_instruction(const char* name, chunk* c, int offset)
{
  uint8_t slot = c->code[offset+1];
  printf("%-16s %4d\n", name, slot);
  return offset + 2;
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
  case OP_HOOK:
    // return constant_instruction("OP_HOOK", c, offset);
    return byte_instruction("OP_HOOK", c, offset);
  case OP_TAG:
    return constant_instruction("OP_TAG", c, offset);
  case OP_GET_VARIABLE:
    return constant_instruction("OP_GET_VARIABLE", c, offset);
  case OP_SET_VARIABLE:
    return constant_instruction("OP_SET_VARIABLE", c, offset);
  case OP_DEFINE_VARIABLE:
    return constant_instruction("OP_DEFINE_VARIABLE", c, offset);
  case OP_POP:
    return simple_instruction("OP_POP", offset);
  case OP_PRINT_LINEBREAK:
    return simple_instruction("OP_PRINT_LINEBREAK", offset);
  case OP_JUMP_IF_FAILED:
    return jump_instruction("OP_JUMP_IF_FAILED", 1, c, offset);
  case OP_PRINT_LINE:
    return constant_instruction("OP_PRINT_LINE", c, offset);
  case OP_NIL:
    return simple_instruction("OP_NIL", offset);
  case OP_STEP_RESULT:
    return constant_instruction("OP_STEP_RESULT", c, offset);
  case OP_PRINT_LOCATION:
    return constant_instruction("OP_PRINT_LOCATION", c, offset);
  case OP_CALL:
    return byte_instruction("OP_CALL", c, offset);
  case OP_CALL_STEP:
    return constant_instruction("OP_CALL_STEP", c, offset);
  case OP_SCENARIO_RESULT:
    return simple_instruction("OP_SCENARIO_RESULT", offset);
  case OP_RETURN:
    return simple_instruction("OP_RETURN", offset);
  default:
    printf("Unknown op code %d\n", instruction);
    return offset+1;
  }
}

