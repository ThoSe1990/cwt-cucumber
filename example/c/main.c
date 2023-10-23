
#include "cwt/cucumber.h"
#include <stdio.h>

static void step_pass(int arg_count, cuke_value* args)
{
  cuke_assert(true);
}
static void step_fail(int arg_count, cuke_value* args)
{
  cuke_assert(false);
}
static void step_test(int arg_count, cuke_value* args)
{
  printf("Running step with %d args: %d and '%s'\n", arg_count, cuke_to_int(&args[0]), cuke_to_string(&args[1]));
}

int main(int argc, const char* argv[])
{
  open_cucumber();

  cuke_step("my step test with {int} and {string}", step_test);
  cuke_step("this passes", step_pass);
  cuke_step("this fails", step_fail);

  run_cuke_argc_argv(argc, argv);
  
  close_cucumber();

  return 0;
}