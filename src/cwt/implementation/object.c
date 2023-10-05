#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"
#include "vm.h"


#define ALLOCATE_OBJ(type, object_type) \
  (type*)allocate_object(sizeof(type), object_type)

static obj* allocate_object(size_t size, obj_type type) 
{
  obj* object = (obj*)reallocate(NULL, 0, size);
  object->type = type;
  object->next = g_vm.objects;
  g_vm.objects = object;
  return object;
}

obj_function* new_function()
{
  obj_function* func = ALLOCATE_OBJ(obj_function, OBJ_FUNCTION);
  func->arity = 0;
  func->name = NULL;
  init_chunk(&func->chunk);
  return func;
}

obj_native* new_native(cuke_step_t function)
{
  obj_native* native = ALLOCATE_OBJ(obj_native, OBJ_NATIVE);
  native->function = function;
  return native;
}

static obj_string* allocate_string(char* chars, int length, uint32_t hash)
{
  obj_string* str = ALLOCATE_OBJ(obj_string, OBJ_STRING);
  str->length = length;
  str->chars = chars;
  str->hash = hash;
  table_set(&g_vm.strings, str, NIL_VAL);
  return str;
}

static uint32_t hash_string(const char* key, int length)
{
  uint32_t hash = 2166136261u;
  for (int i = 0 ; i < length; i++)
  {
    hash ^= (uint8_t)key[i];
    hash *= 16777619;
  }
  return hash;
}

obj_string* take_string(char* chars, int length)
{
  uint32_t hash = hash_string(chars, length);
  obj_string* interned = table_find_string(&g_vm.strings, chars, length, hash);
  if (interned != NULL)
  {
    FREE_ARRAY(char, chars, length+1);
    return interned;
  }
  else 
  {
    return allocate_string(chars, length, hash);
  }
}
obj_string* copy_string(const char* chars, int length)
{
  uint32_t hash = hash_string(chars, length);
  obj_string* interned = table_find_string(&g_vm.strings, chars, length, hash);
  if (interned != NULL) 
  {
    return interned;
  } 
  else 
  {
    char* heap_chars = ALLOCATE(char, length+1);
    memcpy(heap_chars, chars, length);
    heap_chars[length] = '\0';
    return allocate_string(heap_chars, length, hash);
  }
}

static void print_function(obj_function* func) 
{
  if (func->name == NULL)
  {
    printf("<script>");
  }
  else 
  {
    printf("<fn %s>", func->name->chars);
  }
}

void print_object(cuke_value value)
{
  switch(OBJ_TYPE(value))
  {
    case OBJ_FUNCTION: print_function(AS_FUNCTION(value));
    break; case OBJ_STRING: printf("%s", AS_CSTRING(value));
    break; case OBJ_NATIVE: printf("step");
  }
}