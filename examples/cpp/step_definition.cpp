
#include "cwt/cucumber.hpp"

#include "box.hpp"

STEP(box_init, "A box with {int} x {int} x {int}")
{
  const unsigned int w = CUKE_ARG(1);
  const unsigned int l = CUKE_ARG(2);
  const unsigned int h = CUKE_ARG(3);

  cuke::context<box>(w,l,h);
}

STEP(box_water, "Put {double} liter water into the box")
{
  const double water = CUKE_ARG(1);
  cuke::context<box>().set_weight(water);
}

STEP(box_volume, "The volume is {int}")
{
  const unsigned int volume = CUKE_ARG(1);
  cuke::assert_equal(volume, cuke::context<box>().volume());  
}
STEP(box_weight, "The weight is {double} kg")
{
  const double weight = CUKE_ARG(1);
  cuke::assert_equal(weight, cuke::context<box>().weight());  
}
STEP(box_front, "The front area is {int}")
{
  const unsigned int front = CUKE_ARG(1);
  cuke::assert_equal(front, cuke::context<box>().front());
}
STEP(box_top, "The top area is {int}")
{
  const unsigned int top = CUKE_ARG(1);
  cuke::assert_equal(top, cuke::context<box>().top());
}
STEP(box_side, "The side area is {int}")
{
  const unsigned int side = CUKE_ARG(1);
  cuke::assert_equal(side, cuke::context<box>().side());
}
