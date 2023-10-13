#ifndef cwt_cucumber_object_h
#define cwt_cucumber_object_h

#include "common.h"
#include "value.h"
#include "chunk.h"
#include "tag_scanner.h"

#define OBJ_TYPE(value)     (AS_OBJ(value)->type)

#define IS_STRING(value)    is_obj_type(value, OBJ_STRING)
#define IS_FUNCTION(value)  is_obj_type(value, OBJ_FUNCTION)
#define IS_NATIVE(value)    is_obj_type(value, OBJ_NATIVE)
#define IS_HOOK(value)      is_obj_type(value, OBJ_HOOK)


#define AS_STRING(value)    ((obj_string*)AS_OBJ(value))
#define AS_CSTRING(value)   (((obj_string*)AS_OBJ(value))->chars)
#define AS_FUNCTION(value)  ((obj_function*)AS_OBJ(value))
#define AS_NATIVE(value)    (((obj_native*)AS_OBJ(value))->function)
#define AS_HOOK(value)      ((obj_hook*)AS_OBJ(value))

typedef enum {
  OBJ_FUNCTION,
  OBJ_NATIVE,
  OBJ_STRING,
  OBJ_HOOK
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

typedef void (*cuke_step_t)(int arg_count, cuke_value* args);

typedef struct 
{
  obj object;
  cuke_step_t function;
} obj_native;


struct obj_string 
{
  obj object;
  int length;
  char* chars;
  uint32_t hash;
};

typedef struct 
{
  obj object;
  cuke_step_t function;
  cuke_value rpn_tags[TAGS_RPN_MAX];
  unsigned int rpn_size;
} obj_hook;


obj_string* take_string(char* chars, int length);
obj_native* new_native(cuke_step_t function);
obj_hook* new_hook(cuke_step_t function, const char* tag_expression);
obj_function* new_function();
obj_string* copy_string(const char* chars, int length);
void print_object(cuke_value value);

static inline bool is_obj_type(cuke_value value, obj_type type) 
{
  return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif 