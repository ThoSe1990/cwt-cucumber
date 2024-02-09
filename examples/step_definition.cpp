#include "../src/cwt/cucumber.hpp"

BEFORE_T(tag_1, "@tag1")
{
  std::cout << "tag1" << std::endl;
}
BEFORE_T(tag_2_3, "not @tag1 and (@tag2 or @tag3)")
{
  std::cout << "tag_2_3 tag" << std::endl;
}

GIVEN(first_step, "Hello World")
{
  std::cout << "this is my first step" << std::endl;
}
GIVEN(will_fail, "This fails")
{
  cuke::is_true(false);
}

GIVEN(with_value, "{int} is greater 15")
{
  int i = CUKE_ARG(1);
  cuke::greater(i, 15);
}