#include "cucumber.hpp"
#include "box.hpp"


BEFORE(before)
{
  // this runs before every scenario
}
AFTER(after)
{
  // this runs after every scenario
}
BEFORE_STEP(before_step)
{
  // this runs before every step
}
AFTER_STEP(after_step)
{
  // this runs after every step
}

AFTER(close_boxes)
{
  cuke::context<box>().close();
}

AFTER_T(dispatch_box, "@ship or @important")
{
  std::cout << "The box is shipped!" << std::endl;
}

