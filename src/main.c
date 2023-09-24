
#include "cwt_cucumber.h"



static void step_test(int arg_count, value* args)
{
  printf("Running step with: %d and '%s'\n", value_as_int(&args[0]), value_as_string(&args[1]));
}

int main(int argc, const char* argv[])
{
  open_cucumber();

  define_step("my step test with {int} and {string}", step_test);

  run_cucumber(argc, argv);
  
  close_cucumber();

  return 0;
}