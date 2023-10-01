#include <gtest/gtest.h>

extern "C" {
  #include "step_matcher.h"
}

TEST(step_matcher, parse_step)
{
  EXPECT_TRUE(parse_step("foo", "foo", NULL));
}
TEST(step_matcher, step_with_string)
{
  EXPECT_TRUE(parse_step("a step with a {string} inside", "a step with a \"string value\" inside", NULL));
  EXPECT_TRUE(parse_step("{string} at the beginning", "\"string value\" at the beginning", NULL));
  EXPECT_TRUE(parse_step("or a trailing {string}", "or a trailing \"string value\"", NULL));
  EXPECT_TRUE(parse_step("{string} at the beginning, {string} inside and a trailing {string}", 
                      "\"string value\" at the beginning, \"string value\" inside and a trailing \"string value\"", NULL));
}
TEST(step_matcher, step_with_int)
{
  EXPECT_TRUE(parse_step("a step with a {int} inside", "a step with a 123 inside", NULL));
  EXPECT_TRUE(parse_step("a step with a {double} inside", "a step with a 123.456 inside", NULL));
  EXPECT_TRUE(parse_step("{int} at the beginning", "15 at the beginning", NULL));
  EXPECT_TRUE(parse_step("or a trailing {double}", "or a trailing 55.123", NULL));
  EXPECT_TRUE(parse_step("{double} at the beginning, {int} inside and a trailing {double}", 
                      "12.12 at the beginning, 1212 inside and a trailing 9999.1", NULL));
}
TEST(step_matcher, step_with_string_and_int)
{
  EXPECT_TRUE(parse_step("a step with a {string} and {int} inside", "a step with a \"string value\" and 123 inside", NULL));
}
TEST(step_matcher, wrong_data_type)
{
  EXPECT_FALSE(parse_step("a step with a {string}", "a step with a 123", NULL));
  EXPECT_FALSE(parse_step("a step with a {int}", "a step with a \"hello world\"", NULL));
}
TEST(step_matcher, trailing_spaces)
{
  EXPECT_TRUE(parse_step("a step", "a step  ", NULL));
  EXPECT_TRUE(parse_step("a step  ", "a step", NULL));
  EXPECT_TRUE(parse_step("a step {int}", "a step 1234 ", NULL));
  EXPECT_TRUE(parse_step("a step {int}    ", "a step 1234", NULL));
  EXPECT_TRUE(parse_step("a step {string}", "a step \"hello world\"    ", NULL));
  EXPECT_TRUE(parse_step("a step {string}  ", "a step \"hello world\"", NULL));
}
TEST(step_matcher, beginning_spaces)
{
  EXPECT_TRUE(parse_step("  a step", "a step", NULL));
  EXPECT_TRUE(parse_step("a step", "  a step", NULL));
  EXPECT_TRUE(parse_step("  {int} a step", "456 a step", NULL));
  EXPECT_TRUE(parse_step("{int} a step    ", " 1123 a step", NULL));
  EXPECT_TRUE(parse_step("{string} a step", "   \"hello world\" a step ", NULL));
  EXPECT_TRUE(parse_step("   {string} a step", "\"hello world\" a step ", NULL));
}