#ifndef cwt_cucumber_cwt_cucumber_h
#define cwt_cucumber_cwt_cucumber_h

#include "cucumber_value.h"

typedef enum 
{
  CUKE_SUCCESS = 0,
  CUKE_FAILED,
  CUKE_COMPILE_ERROR,
  CUKE_RUNTIME_ERROR,
} cuke_result;


char* read_file(const char* path);

void open_cucumber();
void close_cucumber();

void cuke_options(int argc, const char* argv[]);
void reset_tags();

cuke_result run_cuke(const char* source, const char* path);
cuke_result run_cuke_argc_argv(int argc, const char* argv[]);

void print_final_result();

typedef void (*cuke_step_t)(int arg_count, cuke_value* args);

void step_function(const char* name, cuke_step_t func);
void cuke_given(const char* name, cuke_step_t func);
void cuke_when(const char* name, cuke_step_t func);
void cuke_then(const char* name, cuke_step_t func);
void cuke_hook(const char*, cuke_step_t func, const char* tag_expression);

void cuke_before_step(cuke_step_t func);
void cuke_after_step(cuke_step_t func);

void cuke_before(cuke_step_t func);
void cuke_after(cuke_step_t func);

void cuke_before_t(cuke_step_t func, const char* tag_expression);
void cuke_after_t(cuke_step_t func, const char* tag_expression);



char cuke_to_byte(cuke_value* value);
short cuke_to_short(cuke_value* value);
int cuke_to_int(cuke_value* value);
long long cuke_to_long(cuke_value* value);
float cuke_to_float(cuke_value* value);
double cuke_to_double(cuke_value* value);
const char* cuke_to_string(cuke_value* value);


void cuke_assert(bool assertion, const char* error_msg, ...);

#endif