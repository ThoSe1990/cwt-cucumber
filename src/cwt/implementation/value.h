#ifndef cwt_cucumber_value_h
#define cwt_cucumber_value_h

#include "common.h"
#include "../cucumber_value.h"


#define IS_BOOL(value)    ((value).type == VAL_BOOL)
#define IS_NIL(value)     ((value).type == VAL_NIL)
#define IS_INT(value)     ((value).type == VAL_LONG)
#define IS_DOUBLE(value)     ((value).type == VAL_DOUBLE)
#define IS_OBJ(value)     ((value).type == VAL_OBJ)

#define AS_BOOL(value)    ((value).as.boolean)
#define AS_LONG(value)  ((value).as.long_value)
#define AS_DOUBLE(value)  ((value).as.double_value)
#define AS_OBJ(value)     ((value).as.object)

#define BOOL_VAL(value)   ((cwtc_value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL       ((cwtc_value){VAL_NIL, {.long_value = 0}})
#define LONG_VAL(value) ((cwtc_value){VAL_LONG, {.long_value = value}})
#define DOUBLE_VAL(value) ((cwtc_value){VAL_DOUBLE, {.double_value = value}})
#define OBJ_VAL(value)    ((cwtc_value){VAL_OBJ, {.object = (obj*)value}})


typedef struct {
  int capacity;
  int count;
  cwtc_value* values;
} value_array;

bool values_equal(cwtc_value a, cwtc_value b);
void init_value_array(value_array* arr);
void write_value_array(value_array* arr, cwtc_value value);
void free_value_array(value_array* arr);
void print_value(cwtc_value value);
#endif 