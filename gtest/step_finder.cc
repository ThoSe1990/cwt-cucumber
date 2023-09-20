#include <gtest/gtest.h>

extern "C" {
  #include "step_matcher.h"
}

TEST(step_matcher, is_step_1)
{
  EXPECT_TRUE(is_step("foo", "foo"));
}
TEST(step_matcher, is_step_2)
{
  EXPECT_TRUE(is_step("a step with a {string} inside", "a step with a \"string value\" inside"));
  EXPECT_TRUE(is_step("{string} at the beginning", "\"string value\" at the beginning"));
  EXPECT_TRUE(is_step("or a trailing {string}", "or a trailing \"string value\""));
  EXPECT_TRUE(is_step("{string} at the beginning, {string} inside and a trailing {string}", 
                      "\"string value\" at the beginning, \"string value\" inside and a trailing \"string value\""));
}