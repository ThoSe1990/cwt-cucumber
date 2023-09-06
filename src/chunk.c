#include "chunk.h"
#include "memory.h"
void init_chunk(chunk* c)
{
  c->count = 0;
  c->capacity = 0;
  c->code = NULL;
  c->lines = NULL;
  init_value_array(&c->constants);
}

void free_chunk(chunk* c)
{
  FREE_ARRAY(uint8_t, c->code, c->capacity);
  FREE_ARRAY(int, c->lines, c->capacity);
  free_value_array(&c->constants);
  init_chunk(c);
}

void write_chunk(chunk* c, uint8_t byte, int line)
{
  if (c->capacity < c->count + 1)
  {
    int old_cap = c->capacity;
    c->capacity = GROW_CAPACITY(old_cap);
    c->code = GROW_ARRAY(uint8_t, c->code, old_cap, c->capacity);
    c->lines = GROW_ARRAY(int, c->lines, old_cap, c->capacity);
  }

  c->code[c->count] = byte;
  c->lines[c->count] = line;
  c->count++;
}

int add_constant(chunk* c, value_t v)
{
  write_value_array(&c->constants, v);
  return c->constants.count - 1;
}