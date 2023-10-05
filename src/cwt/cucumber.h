#ifndef cwt_cucumber_cwt_cucumber_h
#define cwt_cucumber_cwt_cucumber_h

#include "cucumber_value.h"

void open_cucumber();
void run_cucumber(int argc, const char* argv[]);
void close_cucumber();


typedef void (*cuke_step_t)(int arg_count, cuke_value* args);
void cuke_step(const char* name, cuke_step_t func);

char cuke_to_byte(cuke_value* value);
short cuke_to_short(cuke_value* value);
int cuke_to_int(cuke_value* value);
long long cuke_to_long(cuke_value* value);
float cuke_to_float(cuke_value* value);
double cuke_to_double(cuke_value* value);
const char* cuke_to_string(cuke_value* value);


void cuke_assert(bool assertion);

#endif