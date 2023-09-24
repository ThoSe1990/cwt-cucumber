#ifndef cwt_cucumber_value_h
#define cwt_cucumber_value_h

#include "common.h"
#include "cwt_cucumber_value.h"


#define IS_BOOL(v)    ((v).type == VAL_BOOL)
#define IS_NIL(v)     ((v).type == VAL_NIL)
#define IS_INT(v)     ((v).type == VAL_INT)
#define IS_OBJ(v)     ((v).type == VAL_OBJ)

#define AS_BOOL(v)    ((v).as.boolean)
#define AS_INT(v)  ((v).as.int_value)
#define AS_OBJ(v)     ((v).as.object)

#define BOOL_VAL(v)   ((value){VAL_BOOL, {.boolean = v}})
#define NIL_VAL       ((value){VAL_NIL, {.int_value = 0}})
#define INT_VAL(v) ((value){VAL_INT, {.int_value = v}})
#define OBJ_VAL(v)    ((value){VAL_OBJ, {.object = (obj*)v}})


typedef struct {
  int capacity;
  int count;
  value* values;
} value_array;

bool values_equal(value a, value b);
void init_value_array(value_array* arr);
void write_value_array(value_array* arr, value v);
void free_value_array(value_array* arr);
void print_value(value v);
#endif 