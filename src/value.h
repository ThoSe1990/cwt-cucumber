#ifndef cwt_cucumber_value_h
#define cwt_cucumber_value_h


#include "common.h"

typedef double value;

typedef struct {
  int capacity;
  int count;
  value* values;
} value_array;

void init_value_array(value_array* arr);
void write_value_array(value_array* arr, value value);
void free_value_array(value_array* arr);
void print_value(value value);

#endif 