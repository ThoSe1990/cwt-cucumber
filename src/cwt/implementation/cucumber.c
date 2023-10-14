#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../cucumber.h"
#include "vm.h"

void cuke_assert(bool assertion)
{
  if (!assertion) 
  {
    g_vm.scenario_results.last = FAILED;
    g_vm.step_results.last = FAILED;
  }
}

static char* read_file(const char* path) 
{
  FILE* file = fopen(path, "rb");
  if (file == NULL) 
  {
    fprintf(stderr, "Could not open file \"%s\".\n", path);
    exit(74);
  }

  fseek(file, 0L, SEEK_END);
  size_t file_size = ftell(file);
  rewind(file);

  char* buffer = (char*)malloc(file_size+1);
  if (buffer == NULL)
  {
    fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
    exit(74);
  }

  size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
  if (bytes_read < file_size)
  {
    fprintf(stderr, "Could not read file \"%s\".\n", path);
    exit(74);
  }
  buffer[bytes_read] = '\0';
  fclose(file);
  return buffer;
}

static void run_file(const char* path) 
{
  char* source = read_file(path);
  interpret_result result = interpret(source, path, "");
  free(source);
  
  if (result == INTERPRET_COMPILE_ERROR) { exit(65); }
  if (result == INTERPRET_RUNTIME_ERROR) { exit(70); } 
}


void open_cucumber()
{
  init_vm();
}
void run_cucumber(int argc, const char* argv[])
{
  if (argc == 2)
  {
    run_file(argv[1]);
  }
  else 
  {
    fprintf(stderr, "Invalid argc/argv\n");
    exit(64);
  }
}
void close_cucumber()
{
  free_vm();
}

void cuke_step(const char* name, cuke_step_t func)
{
  define_step(name, func);
}
void cuke_hook(const char* name, cuke_step_t func, const char* tag_expression)
{
  define_hook(name, func, tag_expression);
}



char cuke_to_byte(cuke_value* value)
{
  return (char) AS_LONG(*value);
}
short cuke_to_short(cuke_value* value)
{
  return (short) AS_LONG(*value);
}
int cuke_to_int(cuke_value* value)
{
  return (int) AS_LONG(*value);
}
long long cuke_to_long(cuke_value* value)
{
  return AS_LONG(*value);
}
float cuke_to_float(cuke_value* value)
{
  // TODO better error handling, eg. waht if this is a  string 
  return IS_DOUBLE(*value) ? (float)AS_DOUBLE(*value) : (float)AS_LONG(*value);
}
double cuke_to_double(cuke_value* value)
{
  // TODO better error handling, eg. waht if this is a  string 
  return IS_DOUBLE(*value) ? AS_DOUBLE(*value) : (double)AS_LONG(*value);
}
const char* cuke_to_string(cuke_value* value)
{
  return AS_STRING(*value)->chars;
}