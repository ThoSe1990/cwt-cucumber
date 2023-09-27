#ifndef cwt_cucumber_cwt_cucumber_h
#define cwt_cucumber_cwt_cucumber_h

#include "cucumber_value.h"

void open_cucumber();
void run_cucumber(int argc, const char* argv[]);
void close_cucumber();


typedef void (*cwtc_step_t)(int arg_count, cwtc_value* args);
void cwtc_step(const char* name, cwtc_step_t func);

int cwtc_to_int(cwtc_value* value);
const char* cwtc_to_string(cwtc_value* value);

void cwtc_assert(bool assertion);

#endif