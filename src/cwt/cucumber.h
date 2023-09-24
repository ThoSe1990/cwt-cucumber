#ifndef cwt_cucumber_cwt_cucumber_h
#define cwt_cucumber_cwt_cucumber_h

#include "cucumber_value.h"

void open_cucumber();
void run_cucumber(int argc, char** argv);
void close_cucumber();


typedef void (*cwtc_step)(int arg_count, cwtc_value* args);
void define_step(const char* name, cwtc_step func);

int value_as_int(cwtc_value* value);
const char* value_as_string(cwtc_value* value);

void cwtc_true(bool assertion);
void cwtc_false(bool assertion);

#endif