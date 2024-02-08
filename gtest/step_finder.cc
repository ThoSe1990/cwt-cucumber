#include <gtest/gtest.h>

#include "../src/cwt/step_finder.hpp"

using namespace cwt::details;

TEST(step_finder, step_finder)
{
  EXPECT_TRUE(step_finder("foo", "foo").step_matches());
}
TEST(step_finder, only_double_1)
{
  EXPECT_TRUE(step_finder("{double}", "123.1").step_matches());
}
TEST(step_finder, only_double_2)
{
  EXPECT_TRUE(step_finder("{double}", "123.123").step_matches());
}
TEST(step_finder, only_double_failing)
{
  EXPECT_FALSE(step_finder("{double}", "hello").step_matches());
}
TEST(step_finder, step_with_string)
{
  EXPECT_TRUE(step_finder("a step with a {string} inside",
                          "a step with a \"string value\" inside")
                  .step_matches());
  EXPECT_TRUE(step_finder("{string} at the beginning",
                          "\"string value\" at the beginning")
                  .step_matches());
  EXPECT_TRUE(
      step_finder("or a trailing {string}", "or a trailing \"string value\"")
          .step_matches());
  EXPECT_TRUE(
      step_finder(
          "{string} at the beginning, {string} inside and a trailing {string}",
          "\"string value\" at the beginning, \"string value\" inside and a "
          "trailing \"string value\"")
          .step_matches());
}
TEST(step_finder, step_with_numbers)
{
  EXPECT_TRUE(
      step_finder("a step with a {int} inside", "a step with a 123 inside")
          .step_matches());
  EXPECT_TRUE(step_finder("a step with a {double} inside",
                          "a step with a 123.456 inside")
                  .step_matches());
  EXPECT_TRUE(step_finder("{int} at the beginning", "15 at the beginning")
                  .step_matches());
  EXPECT_TRUE(step_finder("or a trailing {double}", "or a trailing 55.123")
                  .step_matches());
  EXPECT_TRUE(
      step_finder(
          "{double} at the beginning, {int} inside and a trailing {double}",
          "12.12 at the beginning, 1212 inside and a trailing 9999.1")
          .step_matches());
}
TEST(step_finder, step_with_string_and_int)
{
  EXPECT_TRUE(step_finder("a step with a {string} and {int} inside",
                          "a step with a \"string value\" and 123 inside")
                  .step_matches());
}
TEST(step_finder, wrong_data_type)
{
  EXPECT_FALSE(step_finder("a step with a {string}", "a step with a 123")
                   .step_matches());
  EXPECT_FALSE(
      step_finder("a step with a {int}", "a step with a \"hello world\"")
          .step_matches());
}
TEST(step_finder, trailing_spaces)
{
  EXPECT_TRUE(step_finder("a step", "a step  ").step_matches());
  EXPECT_TRUE(step_finder("a step  ", "a step").step_matches());
  EXPECT_TRUE(step_finder("a step {int}", "a step 1234 ").step_matches());
  EXPECT_TRUE(step_finder("a step {int}    ", "a step 1234").step_matches());
  EXPECT_TRUE(step_finder("a step {string}", "a step \"hello world\"    ")
                  .step_matches());
  EXPECT_TRUE(step_finder("a step {string}  ", "a step \"hello world\"")
                  .step_matches());
}
TEST(step_finder, beginning_spaces)
{
  EXPECT_TRUE(step_finder("  a step", "a step").step_matches());
  EXPECT_TRUE(step_finder("a step", "  a step").step_matches());
  EXPECT_TRUE(step_finder("  {int} a step", "456 a step").step_matches());
  EXPECT_TRUE(step_finder("{int} a step    ", " 1123 a step").step_matches());
  EXPECT_TRUE(step_finder("{string} a step", "   \"hello world\" a step ")
                  .step_matches());
  EXPECT_TRUE(step_finder("   {string} a step", "\"hello world\" a step ")
                  .step_matches());
}
TEST(step_finder, three_ints)
{
  EXPECT_TRUE(
      step_finder("A box with {int} x {int} x {int}", "A box with 2 x 2 x 2")
          .step_matches());
}
TEST(step_finder, chinese_letters)
{
  EXPECT_TRUE(
      step_finder("一个盒子有 {int} x {int} x {int}", "一个盒子有 2 x 2 x 2")
          .step_matches());
}
TEST(step_finder, step_with_variable)
{
  EXPECT_TRUE(step_finder("A Step with a {string}", "A Step with a <value>")
                  .step_matches());
}
TEST(step_finder, value_int)
{
  step_finder sf("  {int} a step", "456 a step");
  EXPECT_TRUE(sf.step_matches());
  EXPECT_EQ(sf.get_value(0).as<int>(), 456);
}
TEST(step_finder, value_double)
{
  step_finder sf("{double} a step", "1.123 a step");
  EXPECT_TRUE(sf.step_matches());
  EXPECT_EQ(sf.get_value(0).as<double>(), 1.123);
}
TEST(step_finder, value_float)
{
  step_finder sf("{float} a step", "1.1 a step");
  EXPECT_TRUE(sf.step_matches());
  // TODO underlying type of {float} or {double} is always a double
  // since the underlying type is always double, the casting to
  // a 32 bit float does not work by now. as workaround accessing
  // a double with an implicit cast works.
  float f = sf.get_value(0).as<double>();
  EXPECT_EQ(f, 1.1f);
}
TEST(step_finder, value_string)
{
  step_finder sf("{string} a step", "\"hello world\" a step");
  EXPECT_TRUE(sf.step_matches());
  EXPECT_EQ(sf.get_value(0).as<std::string>(), std::string("hello world"));
}
TEST(step_finder, step_with_doc_string)
{
  const char* doc_string_step = R"*(this is an arbitrary step with a doc string
"""
this is a doc string
which just follows after the step!
""")*";
  step_finder sf("this is an arbitrary step with a doc string",
                 doc_string_step);
  ASSERT_TRUE(sf.step_matches());
  EXPECT_EQ(
      sf.get_value(0).as<std::string>(),
      std::string("\nthis is a doc string\nwhich just follows after the step!\n"));
}
TEST(step_finder, multiple_values)
{
  step_finder sf("{int} and {double} and {float} and {byte} and {string} and {short}", "1 and 2.2 and 3.3 and 4 and \"five\" and 6");
  EXPECT_TRUE(sf.step_matches());
  EXPECT_EQ(sf.get_value(0).as<int>(), 1);
  EXPECT_EQ(sf.get_value(1).as<double>(), 2.2);
  EXPECT_EQ(sf.get_value(2).as<double>(), 3.3);
  EXPECT_EQ(sf.get_value(3).as<char>(), 4);
  EXPECT_EQ(sf.get_value(4).as<std::string>(), std::string("five"));
  EXPECT_EQ(sf.get_value(5).as<short>(), 6);
}
TEST(step_finder, multiple_values_with_doc_string)
{
  const char* doc_string_step = R"*(1 and 2.2 and 3.3 and 4 and "five" and 6
"""
seven
""")*";
  step_finder sf("{int} and {double} and {float} and {byte} and {string} and {short}", doc_string_step);
  EXPECT_TRUE(sf.step_matches());
  EXPECT_EQ(sf.get_value(6).as<std::string>(), std::string("\nseven\n"));
}