#ifndef cwt_cucumber_compiler_h
#define cwt_cucumber_compiler_h

#include "chunk.h"
#include "object.h"
#include "vm.h"

void set_tag_option(const char* expression);
void set_quiet();
void reset_rpn_stack();

obj_function* compile(const char* source, const char* filename);


#endif 