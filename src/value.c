#include <stdio.h>
#include <string.h>

#include "object.h"
#include "memory.h"
#include "value.h"

bool values_equal(value a, value b)
{
  if (a.type != b.type) { return false; }
  switch (a.type)
  {
    case VAL_BOOL: return AS_BOOL(a) == AS_BOOL(b);
    case VAL_NIL: return true; 
    case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
    case VAL_OBJ: return AS_OBJ(a) == AS_OBJ(b);
    default: return false; // unreachable 
  }
}

void init_value_array(value_array* arr)
{
  arr->values = NULL;
  arr->capacity = 0;
  arr->count = 0;
}
void write_value_array(value_array* arr, value v)
{
  if (arr->capacity < arr->count+1)
  {
    int old_cap = arr->capacity;
    arr->capacity = GROW_CAPACITY(old_cap);
    arr->values = GROW_ARRAY(value, arr->values, old_cap, arr->capacity);
  }
  arr->values[arr->count] = v;
  arr->count++;
}
void free_value_array(value_array* arr)
{
  FREE_ARRAY(value, arr->values, arr->capacity);
  init_value_array(arr);
}

void print_value(value v)
{
  switch (v.type)
  {
  case VAL_BOOL: printf(AS_BOOL(v) ? "true" : "false");
  break; case VAL_NIL: printf("nil"); 
  break; case VAL_NUMBER: printf("%g", AS_NUMBER(v));
  break; case VAL_OBJ: print_object(v); 
  break;
  }
}