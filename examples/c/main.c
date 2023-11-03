
#include "box.h"
#include "cwt/cucumber.h"
#include "step_definition.h"


int main(int argc, const char* argv[])
{
  open_cucumber();

  cuke_step("A box with {int} x {int} x {int}", step_init);
  
  cuke_step("Put {double} liter water into the box", step_water);

  cuke_step("The volume is {int}", step_volume);
  cuke_step("The weight is {double} kg", step_weight);
  cuke_step("The front area is {int}", step_front);
  cuke_step("The side area is {int}", step_side);
  cuke_step("The top area is {int}", step_top);


  run_cuke_argc_argv(argc, argv);
  
  close_cucumber();

  return 0;
}