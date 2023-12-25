#ifndef cwt_cucumber_compiler_h
#define cwt_cucumber_compiler_h

#include "chunk.h"
#include "object.h"
#include "vm.h"

typedef enum 
{
  PRINT_WHITE = 0,
  PRINT_BLACK
} print_color;

void init_compiler_options();

void set_tag_option(const char* expression);
void only_compile_lines(const int* lines, int count);
void compile_all();
void set_quiet();
void reset_rpn_stack();

obj_function* compile(const char* source, const char* filename);


#endif 