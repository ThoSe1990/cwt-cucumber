
#include "cwt/cucumber.hpp"

#include "box.hpp"

AFTER_T(after_t, "@tag1 or @tag2")
{
  // std::cout << "hook after  @tag1 or @tag2 " << std::endl;
}
AFTER(after)
{
  // std::cout << "hook after for all!!!!! " << std::endl;
}

BEFORE_T(before_t, "@tag1")
{
  // std::cout << "hook BEFORE  @tag1 " << std::endl;
}
BEFORE(before)
{
  // std::cout << "hook BEFORE for all!!!!! " << std::endl;
}

BEFORE_STEP(before_step)
{
  // std::cout << "hook BEFORE_STEP" << std::endl;
}
AFTER_STEP(before_step)
{
  // std::cout << "hook AFTER_STEP" << std::endl;
}

STEP(hi, "hi")
{
  std::cout << "Hi" << std::endl;
}

STEP(box_init, "A box with {int} x {int} x {int}")
{
  const unsigned int w = CUKE_ARG(1);
  const unsigned int l = CUKE_ARG(2);
  const unsigned int h = CUKE_ARG(3);

  cuke::context<box>(w,l,h);
}

STEP(box_water, "Put {int} liter water into the box")
{
  const unsigned int water = CUKE_ARG(1);
  cuke::context<box>().set_weight(water);
}

STEP(box_volume, "The volume is {int}")
{
  const unsigned int volume = CUKE_ARG(1);
  cuke::assert_equal(volume, cuke::context<box>().volume());  
}
STEP(box_weight, "The weight is {int} kg")
{
  const unsigned int weight = CUKE_ARG(1);
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
