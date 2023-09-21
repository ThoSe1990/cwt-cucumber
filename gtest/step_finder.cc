#include <gtest/gtest.h>

extern "C" {
  #include "step_matcher.h"
}

TEST(step_matcher, is_step)
{
  EXPECT_TRUE(is_step("foo", "foo"));
}
TEST(step_matcher, is_step_with_string)
{
  EXPECT_TRUE(is_step("a step with a {string} inside", "a step with a \"string value\" inside"));
  EXPECT_TRUE(is_step("{string} at the beginning", "\"string value\" at the beginning"));
  EXPECT_TRUE(is_step("or a trailing {string}", "or a trailing \"string value\""));
  EXPECT_TRUE(is_step("{string} at the beginning, {string} inside and a trailing {string}", 
                      "\"string value\" at the beginning, \"string value\" inside and a trailing \"string value\""));
}
TEST(step_matcher, is_step_with_int)
{
  EXPECT_TRUE(is_step("a step with a {int} inside", "a step with a 123 inside"));
  EXPECT_TRUE(is_step("a step with a {int} inside", "a step with a 123.456 inside"));
  EXPECT_TRUE(is_step("{int} at the beginning", "15 at the beginning"));
  EXPECT_TRUE(is_step("or a trailing {int}", "or a trailing 55.123"));
  EXPECT_TRUE(is_step("{int} at the beginning, {int} inside and a trailing {int}", 
                      "12.12 at the beginning, 1212 inside and a trailing 9999.1"));
}