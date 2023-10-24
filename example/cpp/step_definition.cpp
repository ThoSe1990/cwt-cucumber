
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

STEP(box_volume, "The volume is {int}")
{
  const unsigned volume = CUKE_ARG(1);
  cuke::assert_equal(volume, cuke::context<box>().volume());  
}