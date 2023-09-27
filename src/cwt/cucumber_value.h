#ifndef cwt_cucumber_cwt_cucumber_value_h
#define cwt_cucumber_cwt_cucumber_value_h

#include <stdbool.h>

typedef struct obj obj;
typedef struct obj_string obj_string;

typedef enum {
  VAL_BOOL,
  VAL_NIL,
  VAL_LONG,
  VAL_DOUBLE,
  VAL_OBJ 
} cwtc_value_type;

typedef struct {
  cwtc_value_type type;
  union {
    bool boolean;
    long long long_value;
    double double_value;
    obj* object; 
  } as;
} cwtc_value;

#endif