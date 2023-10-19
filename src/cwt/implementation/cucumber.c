#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../cucumber.h"
#include "vm.h"
#include "compiler.h"
#include "prints.h"

static void print_results(const char* type, result_t* result)
{
  const int count = result->failed + result->passed +
                   result->skipped + result->undefined; 

  printf("%d %s (", count, type);

  bool print_coma = false;
  if (result->failed > 0)
  {
    print_red("%d failed", result->failed);
    print_coma = true;
  }
  if (result->undefined > 0)
  {
    if (print_coma) printf(", ");
    print_yellow("%d undefined", result->undefined);
    print_coma = true;
  }
  if (result->skipped > 0)
  {
    if (print_coma) printf(", ");
    print_blue("%d skipped", result->skipped);
    print_coma = true;
  }
  if (result->passed > 0)
  {
    if (print_coma) printf(", ");
    print_green("%d passed", result->passed);
    print_coma = true;
  }
  printf(")\n");
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

void set_program_options(int argc, const char* argv[])
{
  for (int i = 1; i < argc; i++) 
  {
    if (strcmp(argv[i], "--tags") == 0 || strcmp(argv[i], "-t") == 0) 
    {
      if (i+1 < argc)
      {
        set_tag_option(argv[i+1]);
      }
      else 
      {
        fprintf(stderr, "Expect value after '%s'.", argv[i]);
      }
    } 
    else if (*argv[i] == '-')
    {
      fprintf(stderr, "Unknown program option: '%s'.", argv[i]);
    }
  }
}

static int run_all_files(int argc, const char* argv[])
{
  int result = CUKE_SUCCESS;
  for (int i = 1; i < argc; i++) 
  {
    if (*argv[i] == '-')
    {
      i++;
      continue;
    }
    char* source = read_file(argv[i]);
    if (run_cuke(source, argv[i]) == CUKE_FAILED)
    {
      result = CUKE_FAILED;
    }
    free(source);
  }
  
  print_results("Scenarios", get_scenario_result());
  print_results("Steps", get_step_result());

  return result;
}


int run_cuke(const char* source, const char* path) 
{
  interpret_result result = interpret(source, path);
  
  if (result == INTERPRET_COMPILE_ERROR) { return CUKE_FAILED; }
  if (result == INTERPRET_RUNTIME_ERROR) { return CUKE_FAILED; } 
  return CUKE_SUCCESS;
}


void open_cucumber()
{
  init_vm();
}

int run_cuke_from_argv(int argc, const char* argv[])
{
  set_program_options(argc, argv);
  return run_all_files(argc, argv);
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

void cuke_before_step(cuke_step_t func)
{
  cuke_hook("before_step", func, "");
}
void cuke_after_step(cuke_step_t func)
{
  cuke_hook("after_step", func, "");
}

void cuke_before(cuke_step_t func)
{
  cuke_hook("before", func, "");
}
void cuke_after(cuke_step_t func)
{
  cuke_hook("after", func, "");
}

void cuke_before_t(cuke_step_t func, const char* tag_expression)
{
  cuke_hook("before", func, tag_expression);
}
void cuke_after_t(cuke_step_t func, const char* tag_expression)
{
  cuke_hook("after", func, tag_expression);
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

void cuke_assert(bool assertion)
{
  if (!assertion) 
  {
    g_vm.scenario_results.last = FAILED;
    g_vm.step_results.last = FAILED;
  }
}
