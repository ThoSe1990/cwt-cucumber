#include <stdio.h>

#include "memory.h"
#include "value.h"

void init_value_array(value_array* arr)
{
  arr->values = NULL;
  arr->capacity = 0;
  arr->count = 0;
}
void write_value_array(value_array* arr, value_t value)
{
  if (arr->capacity < arr->count+1)
  {
    int old_cap = arr->capacity;
    arr->capacity = GROW_CAPACITY(old_cap);
    arr->values = GROW_ARRAY(value_t, arr->values, old_cap, arr->capacity);
  }
  arr->values[arr->count] = value;
  arr->count++;
}
void free_value_array(value_array* arr)
{
  FREE_ARRAY(value_t, arr->values, arr->capacity);
  init_value_array(arr);
}

void print_value(value_t value)
{
  printf("%g", value);
}