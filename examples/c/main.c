
#include "box.h"
#include "cwt/cucumber.h"
#include "step_definition.h"


int main(int argc, const char* argv[])
{
  open_cucumber();

  cuke_given("A box with {int} x {int} x {int}", step_init);

  cuke_when("I open the box", step_open);
  cuke_when("I close the box", step_close);
  cuke_when("Put {double} liter water into the box", step_water);
      
  cuke_when("The box's name is {string}", step_set_name);
  cuke_when("The box's description is", step_set_description);

  cuke_then("The box is open", step_is_open);
  cuke_then("The box is closed", step_is_closed);
  cuke_then("The volume is {long}", step_volume);
  cuke_then("The weight is {double} kg", step_weight);
  cuke_then("The front area is {int}", step_front);
  cuke_then("The side area is {int}", step_side);
  cuke_then("The top area is {int}", step_top);

  cuke_then("I print the box's name and description", step_print_box);

  run_cuke_argc_argv(argc, argv);
  
  close_cucumber();

  return 0;
}