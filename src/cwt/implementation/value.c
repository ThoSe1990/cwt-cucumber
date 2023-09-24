#include <stdio.h>
#include <string.h>

#include "object.h"
#include "memory.h"
#include "value.h"

bool values_equal(cwtc_value a, cwtc_value b)
{
  if (a.type != b.type) { return false; }
  switch (a.type)
  {
    case VAL_BOOL: return AS_BOOL(a) == AS_BOOL(b);
    case VAL_NIL: return true; 
    case VAL_INT: return AS_INT(a) == AS_INT(b);
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
void write_value_array(value_array* arr, cwtc_value value)
{
  if (arr->capacity < arr->count+1)
  {
    int old_cap = arr->capacity;
    arr->capacity = GROW_CAPACITY(old_cap);
    arr->values = GROW_ARRAY(cwtc_value, arr->values, old_cap, arr->capacity);
  }
  arr->values[arr->count] = value;
  arr->count++;
}
void free_value_array(value_array* arr)
{
  FREE_ARRAY(cwtc_value, arr->values, arr->capacity);
  init_value_array(arr);
}

void print_value(cwtc_value value)
{
  switch (value.type)
  {
  case VAL_BOOL: printf(AS_BOOL(value) ? "true" : "false");
  break; case VAL_NIL: printf("nil"); 
  break; case VAL_INT: printf("%d", AS_INT(value));
  break; case VAL_OBJ: print_object(value); 
  break;
  }
}