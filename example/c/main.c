
#include "cwt/cucumber.h"


static void step_pass(int arg_count, cwtc_value* args)
{
  cwtc_assert_true(true);
}
static void step_fail(int arg_count, cwtc_value* args)
{
  cwtc_assert_true(false);
}
static void step_test(int arg_count, cwtc_value* args)
{
  printf("Running step with %d args: %d and '%s'\n", arg_count, cwtc_to_int(&args[0]), cwtc_to_string(&args[1]));
}

int main(int argc, const char* argv[])
{
  open_cucumber();

  define_step("my step test with {int} and {string}", step_test);
  define_step("this passes", step_pass);
  define_step("this fails", step_fail);

  run_cucumber(argc, argv);
  
  close_cucumber();

  return 0;
}