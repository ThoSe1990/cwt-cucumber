#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../cucumber.h"
#include "vm.h"

void cwtc_assert_true(bool assertion)
{
  if (!assertion) 
  {
    g_vm.last_result = STEP_FAILED;
  }
}
void cwtc_assert_false(bool assertion)
{
  if (assertion) 
  {
    g_vm.last_result = STEP_FAILED;
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
  interpret_result result = interpret(source, path);
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

void define_step(const char* name, cwtc_step func)
{
  define_native(name, func);
}


int cwtc_to_int(cwtc_value* value)
{
  return (int)AS_INT(*value);
}
const char* cwtc_to_string(cwtc_value* value)
{
  return AS_STRING(*value)->chars;
}