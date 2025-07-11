#include "cucumber.hpp"
#include "box.hpp"

BEFORE_T(skip, "@skip") { cuke::skip_scenario(); }
BEFORE_T(ignore, "@ignore") { cuke::ignore_scenario(); }
BEFORE_T(will_fail_before, "@will_fail_before")
{
  cuke::fail_scenario(
      "Example of 'cuke::fail_scenario()' to bring a scenario to fail before "
      "running it");
}
AFTER_T(will_fail_after, "@will_fail_after")
{
  cuke::fail_scenario(
      "Example of 'cuke::fail_scenario()' to bring a scenario to fail after "
      "running it");
}

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

AFTER(close_boxes) { cuke::context<box>().close(); }

AFTER_T(dispatch_box, "@ship or @important")
{
  std::cout << "The box is shipped!" << std::endl;
}

BEFORE_ALL(before_all) { std::cout << "-- Hook before all" << std::endl; }

AFTER_ALL(after_all) { std::cout << "-- Hook after all" << std::endl; }
