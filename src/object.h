#ifndef cwt_cucumber_object_h
#define cwt_cucumber_object_h

#include "common.h"
#include "value.h"
#include "chunk.h"


#define OBJ_TYPE(v)     (AS_OBJ(v)->type)

#define IS_STRING(v)    is_obj_type(v, OBJ_STRING)
#define IS_FUNCTION(v)  is_obj_type(v, OBJ_FUNCTION)
#define IS_NATIVE(v)    is_obj_type(v, OBJ_NATIVE)


#define AS_STRING(v)    ((obj_string*)AS_OBJ(v))
#define AS_CSTRING(v)   (((obj_string*)AS_OBJ(v))->chars)
#define AS_FUNCTION(v)  ((obj_function*)AS_OBJ(v))
#define AS_NATIVE(v)    (((obj_native*)AS_OBJ(v))->function)

typedef enum {
  OBJ_FUNCTION,
  OBJ_NATIVE,
  OBJ_STRING
} obj_type;

struct obj {
  obj_type type;
  struct obj* next;
};

typedef struct {
  obj object;
  int arity;
  chunk chunk;
  obj_string* name;
} obj_function;

typedef value (*native_func)(int arg_count, value* args);

typedef struct {
  obj object;
  native_func function;
} obj_native;

struct obj_string {
  obj object;
  int length;
  char* chars;
  uint32_t hash;
};

obj_string* take_string(char* chars, int length);
obj_native* new_native(native_func function);
obj_function* new_function();
obj_string* copy_string(const char* chars, int length);
void print_object(value v);

static inline bool is_obj_type(value v, obj_type type) 
{
  return IS_OBJ(v) && AS_OBJ(v)->type == type;
}

#endif 