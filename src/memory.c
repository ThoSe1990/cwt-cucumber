#include <stdlib.h>

#include "memory.h"
#include "vm.h"

void* reallocate (void* pointer, size_t old_size, size_t new_size)
{
  if (new_size == 0)
  {
    free(pointer);
    return NULL;
  }
  void* result = realloc(pointer, new_size);
  if (result == NULL) { exit(1); }
  return result;
}


void free_object(obj* object)
{
  switch (object->type)
  {

    case OBJ_FUNCTION:
    {
      obj_function* func = (obj_function*)object; 
      free_chunk(&func->chunk);
      FREE(obj_function, object);
    } 
    break; case OBJ_NATIVE:
    {
      printf("<native fn>");
    }
    break; case OBJ_STRING: 
    {
      obj_string* str = (obj_string*)object;
      FREE_ARRAY(char, str->chars, str->length+1);
      FREE(obj_string, object);
    }
  }
}

void free_objects()
{
  obj* object = g_vm.objects;
  while (object != NULL)
  {
    obj* next = object->next;
    free_object(object);
    object = next;
  }
}
