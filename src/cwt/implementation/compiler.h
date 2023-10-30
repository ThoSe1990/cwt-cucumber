#ifndef cwt_cucumber_compiler_h
#define cwt_cucumber_compiler_h

#include "chunk.h"
#include "object.h"
#include "vm.h"

void init_compiler_options();

void set_tag_option(const char* expression);
void set_scenarios_by_line(int* lines, int count);
void reset_scenarios_by_line();
void set_quiet();
void reset_rpn_stack();

obj_function* compile(const char* source, const char* filename);


#endif 