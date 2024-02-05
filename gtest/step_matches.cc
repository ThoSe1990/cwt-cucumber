#include <gtest/gtest.h>


#include "../src/cwt/step_finder.hpp"

using namespace cwt::details;

TEST(step_matcher, step_finder)
{
  EXPECT_TRUE(step_finder("foo", "foo").step_matches());
}
TEST(step_matcher, only_double_1)
{
  EXPECT_TRUE(step_finder("{double}", "123.1").step_matches());
}
TEST(step_matcher, only_double_2)
{
  EXPECT_TRUE(step_finder("{double}", "123.123").step_matches());
}
TEST(step_matcher, only_double_failing)
{
  EXPECT_FALSE(step_finder("{double}", "hello").step_matches());
}
TEST(step_matcher, step_with_string)
{
  EXPECT_TRUE(step_finder("a step with a {string} inside", "a step with a \"string value\" inside").step_matches());
  EXPECT_TRUE(step_finder("{string} at the beginning", "\"string value\" at the beginning").step_matches());
  EXPECT_TRUE(step_finder("or a trailing {string}", "or a trailing \"string value\"").step_matches());
  EXPECT_TRUE(step_finder("{string} at the beginning, {string} inside and a trailing {string}", 
                      "\"string value\" at the beginning, \"string value\" inside and a trailing \"string value\"").step_matches());
}
TEST(step_matcher, step_with_numbers)
{
  EXPECT_TRUE(step_finder("a step with a {int} inside", "a step with a 123 inside").step_matches());
  EXPECT_TRUE(step_finder("a step with a {double} inside", "a step with a 123.456 inside").step_matches());
  EXPECT_TRUE(step_finder("{int} at the beginning", "15 at the beginning").step_matches());
  EXPECT_TRUE(step_finder("or a trailing {double}", "or a trailing 55.123").step_matches());
  EXPECT_TRUE(step_finder("{double} at the beginning, {int} inside and a trailing {double}", 
                      "12.12 at the beginning, 1212 inside and a trailing 9999.1").step_matches());
}
TEST(step_matcher, step_with_string_and_int)
{
  EXPECT_TRUE(step_finder("a step with a {string} and {int} inside", "a step with a \"string value\" and 123 inside").step_matches());
}
TEST(step_matcher, wrong_data_type)
{
  EXPECT_FALSE(step_finder("a step with a {string}", "a step with a 123").step_matches());
  EXPECT_FALSE(step_finder("a step with a {int}", "a step with a \"hello world\"").step_matches());
}
TEST(step_matcher, trailing_spaces)
{
  EXPECT_TRUE(step_finder("a step", "a step  ").step_matches());
  EXPECT_TRUE(step_finder("a step  ", "a step").step_matches());
  EXPECT_TRUE(step_finder("a step {int}", "a step 1234 ").step_matches());
  EXPECT_TRUE(step_finder("a step {int}    ", "a step 1234").step_matches());
  EXPECT_TRUE(step_finder("a step {string}", "a step \"hello world\"    ").step_matches());
  EXPECT_TRUE(step_finder("a step {string}  ", "a step \"hello world\"").step_matches());
}
TEST(step_matcher, beginning_spaces)
{
  EXPECT_TRUE(step_finder("  a step", "a step").step_matches());
  EXPECT_TRUE(step_finder("a step", "  a step").step_matches());
  EXPECT_TRUE(step_finder("  {int} a step", "456 a step").step_matches());
  EXPECT_TRUE(step_finder("{int} a step    ", " 1123 a step").step_matches());
  EXPECT_TRUE(step_finder("{string} a step", "   \"hello world\" a step ").step_matches());
  EXPECT_TRUE(step_finder("   {string} a step", "\"hello world\" a step ").step_matches());
}
TEST(step_matcher, three_ints)
{
  EXPECT_TRUE(step_finder("A box with {int} x {int} x {int}", "A box with 2 x 2 x 2").step_matches());
}
TEST(step_matcher, chinese_letters)
{
  EXPECT_TRUE(step_finder("一个盒子有 {int} x {int} x {int}", "一个盒子有 2 x 2 x 2").step_matches());
}
TEST(step_matcher, step_with_variable)
{
  EXPECT_TRUE(step_finder("A Step with a {string}", "A Step with a <value>").step_matches());
}