
#include "cwt/cucumber.hpp"


STEP(will_fail, "this fails!")
{
  cuke::assert_equal(true, false);
}
