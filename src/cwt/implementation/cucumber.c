#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../cucumber.h"
#include "vm.h"
#include "compiler.h"
#include "prints.h"
#include "program_options.h"

static void print_failed_scenarios()
{
  failed_scenarios_t* failed = get_failed_scenarios();
  if (failed->name.count > 0)
  {
    printf("Failed Scenarios:\n");
  }
  for (int i = 0 ; i < failed->name.count ; i++)
  {
    print_red("  %s", 
      AS_CSTRING(failed->name.values[i]));
    print_black("\t%s\n",  
      AS_CSTRING(failed->location.values[i]));
  }  
}

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

char* read_file(const char* path) 
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


void reset_tags()
{
  reset_rpn_stack();
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

    int lines[MAX_LINES];
    int count = 0;
    option_lines(argc, argv, i, lines, &count);
    only_compile_lines(lines, count);

    if (run_cuke(source, argv[i]) == CUKE_FAILED)
    {
      result = CUKE_FAILED;
    }
    free(source);
  }
  
  print_final_result();

  return result;
}

void open_cucumber()
{
  init_vm();
  init_compiler_options();
}


int run_cuke(const char* source, const char* path) 
{
  interpret_result result = interpret(source, path);
  
  if (result == INTERPRET_COMPILE_ERROR) { return CUKE_FAILED; }
  if (result == INTERPRET_RUNTIME_ERROR) { return CUKE_FAILED; } 
  return CUKE_SUCCESS;
}

void cuke_options(int argc, const char* argv[])
{
  global_options(argc, argv);
}

int run_cuke_argc_argv(int argc, const char* argv[])
{
  return run_all_files(argc, argv);
}

void print_final_result()
{
  print_failed_scenarios();
  printf("\n");
  print_results("Scenarios", get_scenario_result());
  print_results("Steps", get_step_result());
}

void close_cucumber()
{
  free_vm();
}

void cuke_step(const char* name, cuke_step_t func)
{
  define_step(name, func);
}

void cuke_given(const char* name, cuke_step_t func)
{
  cuke_step(name, func);
}
void cuke_when(const char* name, cuke_step_t func)
{
  cuke_step(name, func);
}
void cuke_then(const char* name, cuke_step_t func)
{
  cuke_step(name, func);
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

void cuke_assert(bool assertion, const char* error_msg, ...)
{
  if (!assertion) 
  {
    g_vm.scenario_results.current = FAILED;
    g_vm.step_results.current = FAILED;
    // TODO dry 
    va_list args;
    va_start(args, error_msg);
    printf("\x1b[31m");
    vprintf(error_msg, args);
    printf("\x1b[0m\n");
    va_end(args);
  }
}
