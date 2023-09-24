#ifndef cwt_cucumber_cwt_cucumber_h
#define cwt_cucumber_cwt_cucumber_h

#include "cwt_cucumber_value.h"

void open_cucumber();
void run_cucumber(int argc, char** argv);
void close_cucumber();


typedef void (*native_func)(int arg_count, value* args);
void define_step(const char* name, native_func func);

int value_as_int(value* v);
const char* value_as_string(value* v);

#endif