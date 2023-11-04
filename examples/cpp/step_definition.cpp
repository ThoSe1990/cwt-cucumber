#include "cwt/cucumber.hpp"

#include "box.hpp"

GIVEN(box_init, "A box with {int} x {int} x {int}")
{
  const unsigned int w = CUKE_ARG(1);
  const unsigned int l = CUKE_ARG(2);
  const unsigned int h = CUKE_ARG(3);

  cuke::context<box>(w,l,h);
}

WHEN(box_open, "I open the box")
{
  cuke::context<box>().open(); 
}
WHEN(box_close, "I close the box")
{
  cuke::context<box>().close(); 
}
WHEN(box_water, "Put {double} liter water into the box")
{
  const double water = CUKE_ARG(1);
  cuke::context<box>().set_weight(water);
}

THEN(box_is_open, "The box is open")
{
  cuke::is_true(cuke::context<box>().is_open());
}
THEN(box_is_closed, "The box is closed")
{
  cuke::is_false(cuke::context<box>().is_open());
}
THEN(box_volume, "The volume is {int}")
{
  const unsigned int volume = CUKE_ARG(1);
  cuke::equal(volume, cuke::context<box>().volume());  
}
THEN(box_weight, "The weight is {double} kg")
{
  const double weight = CUKE_ARG(1);
  cuke::equal(weight, cuke::context<box>().weight());  
}
THEN(box_front, "The front area is {int}")
{
  const unsigned int front = CUKE_ARG(1);
  cuke::equal(front, cuke::context<box>().front());
}
THEN(box_top, "The top area is {int}")
{
  const unsigned int top = CUKE_ARG(1);
  cuke::equal(top, cuke::context<box>().top());
}
THEN(box_side, "The side area is {int}")
{
  const unsigned int side = CUKE_ARG(1);
  cuke::equal(side, cuke::context<box>().side());
}
