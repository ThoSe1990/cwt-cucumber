#ifndef cwt_cucumber_value_h
#define cwt_cucumber_value_h

#include "common.h"
#include "../cucumber_value.h"


#define IS_BOOL(value)    ((value).type == VAL_BOOL)
#define IS_NIL(value)     ((value).type == VAL_NIL)
#define IS_INT(value)     ((value).type == VAL_INT)
#define IS_OBJ(value)     ((value).type == VAL_OBJ)

#define AS_BOOL(value)    ((value).as.boolean)
#define AS_INT(value)  ((value).as.int_value)
#define AS_OBJ(value)     ((value).as.object)

#define BOOL_VAL(value)   ((cwtc_value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL       ((cwtc_value){VAL_NIL, {.int_value = 0}})
#define INT_VAL(value) ((cwtc_value){VAL_INT, {.int_value = value}})
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