#ifndef cwt_cucumber_cwt_cucumber_value_h
#define cwt_cucumber_cwt_cucumber_value_h

#include <stdbool.h>

typedef struct obj obj;
typedef struct obj_string obj_string;

typedef enum {
  VAL_BOOL,
  VAL_NIL,
  VAL_INT,
  VAL_OBJ 
} value_type;

typedef struct {
  value_type type;
  union {
    bool boolean;
    int int_value;
    obj* object; 
  } as;
} value;

#endif