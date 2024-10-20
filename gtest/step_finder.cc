#include <gtest/gtest.h>

#include "../src/step_finder.hpp"

using namespace cuke::internal;

TEST(step_finder, step_finder)
{
  EXPECT_TRUE(step_finder("foo").step_matches("foo"));
}
TEST(step_finder, only_double_1)
{
  EXPECT_TRUE(step_finder("123.1").step_matches("{double}"));
}
TEST(step_finder, only_double_2)
{
  EXPECT_TRUE(step_finder("123.123").step_matches("{double}"));
}
TEST(step_finder, only_double_failing)
{
  EXPECT_FALSE(step_finder("hello").step_matches("{double}"));
}
TEST(step_finder, step_with_string)
{
  EXPECT_TRUE(step_finder("a step with a \"string value\" inside")
                  .step_matches("a step with a {string} inside"));
  EXPECT_TRUE(step_finder("\"string value\" at the beginning")
                  .step_matches("{string} at the beginning"));
  EXPECT_TRUE(step_finder("or a trailing \"string value\"")
                  .step_matches("or a trailing {string}"));
  EXPECT_TRUE(step_finder("\"string value\" at the beginning, \"string value\" "
                          "inside and a "
                          "trailing \"string value\"")
                  .step_matches("{string} at the beginning, {string} inside "
                                "and a trailing {string}"));
}
TEST(step_finder, step_with_numbers)
{
  EXPECT_TRUE(step_finder("a step with a 123 inside")
                  .step_matches("a step with a {int} inside"));
  EXPECT_TRUE(step_finder("a step with a 123.456 inside")
                  .step_matches("a step with a {double} inside"));
  EXPECT_TRUE(step_finder("15 at the beginning")
                  .step_matches("{int} at the beginning"));
  EXPECT_TRUE(step_finder("or a trailing 55.123")
                  .step_matches("or a trailing {double}"));
  EXPECT_TRUE(
      step_finder("12.12 at the beginning, 1212 inside and a trailing 9999.1")
          .step_matches("{double} at the beginning, {int} inside and a "
                        "trailing {double}"));
}
TEST(step_finder, step_with_string_and_int)
{
  EXPECT_TRUE(step_finder("a step with a \"string value\" and 123 inside")
                  .step_matches("a step with a {string} and {int} inside"));
}
TEST(step_finder, wrong_data_type)
{
  EXPECT_FALSE(
      step_finder("a step with a 123").step_matches("a step with a {string}"));
  EXPECT_FALSE(step_finder("a step with a \"hello world\"")
                   .step_matches("a step with a {int}"));
}
TEST(step_finder, trailing_spaces)
{
  EXPECT_TRUE(step_finder("a step  ").step_matches("a step"));
  EXPECT_TRUE(step_finder("a step").step_matches("a step  "));
  EXPECT_TRUE(step_finder("a step 1234 ").step_matches("a step {int}"));
  EXPECT_TRUE(step_finder("a step 1234").step_matches("a step {int}    "));
  EXPECT_TRUE(step_finder("a step \"hello world\"    ")
                  .step_matches("a step {string}"));
  EXPECT_TRUE(
      step_finder("a step \"hello world\"").step_matches("a step {string}  "));
}
TEST(step_finder, beginning_spaces)
{
  EXPECT_TRUE(step_finder("a step").step_matches("  a step"));
  EXPECT_TRUE(step_finder("  a step").step_matches("a step"));
  EXPECT_TRUE(step_finder("456 a step").step_matches("  {int} a step"));
  EXPECT_TRUE(step_finder(" 1123 a step").step_matches("{int} a step    "));
  EXPECT_TRUE(step_finder("   \"hello world\" a step ")
                  .step_matches("{string} a step"));
  EXPECT_TRUE(step_finder("\"hello world\" a step ")
                  .step_matches("   {string} a step"));
}
TEST(step_finder, three_ints)
{
  EXPECT_TRUE(step_finder("A box with 2 x 2 x 2")
                  .step_matches("A box with {int} x {int} x {int}"));
}
TEST(step_finder, chinese_letters)
{
  EXPECT_TRUE(step_finder("一个盒子有 2 x 2 x 2")
                  .step_matches("一个盒子有 {int} x {int} x {int}"));
}
TEST(step_finder, step_with_variable)
{
  cuke::value_array data{cuke::value(std::string("value")),
                         cuke::value(std::string("some string value"))};
  cuke::table t(data, 1);

  step_finder sf("A Step with a <value>", t.hash_row(1));
  ASSERT_TRUE(sf.step_matches("A Step with a {string}"));
  ASSERT_EQ(sf.values().size(), 1);
  EXPECT_EQ(sf.values()[0].as<std::string>(), std::string("some string value"));
}
TEST(step_finder, value_int)
{
  step_finder sf("456 a step");
  EXPECT_TRUE(sf.step_matches("  {int} a step"));
  EXPECT_EQ(sf.values().at(0).as<int>(), 456);
}
TEST(step_finder, value_double)
{
  step_finder sf("1.123 a step");
  EXPECT_TRUE(sf.step_matches("{double} a step"));
  EXPECT_EQ(sf.values().at(0).as<double>(), 1.123);
}
TEST(step_finder, value_float)
{
  step_finder sf("1.1 a step");
  EXPECT_TRUE(sf.step_matches("{float} a step"));
  EXPECT_EQ(sf.values().at(0).as<float>(), 1.1f);
}
TEST(step_finder, value_string)
{
  step_finder sf("\"hello world\" a step");
  EXPECT_TRUE(sf.step_matches("{string} a step"));
  EXPECT_EQ(sf.values().at(0).as<std::string>(), std::string("hello world"));
}
TEST(step_finder, multiple_values)
{
  step_finder sf("1 and 2.2 and 3.3 and 4 and \"five\" and 6");
  EXPECT_TRUE(sf.step_matches(
      "{int} and {double} and {float} and {byte} and {string} and {short}"));
  EXPECT_EQ(sf.values().at(0).as<int>(), 1);
  EXPECT_EQ(sf.values().at(1).as<double>(), 2.2);
  EXPECT_EQ(sf.values().at(2).as<float>(), 3.3f);
  EXPECT_EQ(sf.values().at(3).as<char>(), 4);
  EXPECT_EQ(sf.values().at(4).as<std::string>(), std::string("five"));
  EXPECT_EQ(sf.values().at(5).as<short>(), 6);
}

TEST(step_finder, word_at_stepend)
{
  step_finder sf("step with something");
  ASSERT_TRUE(sf.step_matches("step with {word}"));
  EXPECT_EQ(sf.values().size(), 1);
  EXPECT_EQ(sf.values().at(0).as<std::string>(), std::string("something"));
}
TEST(step_finder, word_at_stepbegin)
{
  step_finder sf("something in step");
  ASSERT_TRUE(sf.step_matches("{word} in step"));
  EXPECT_EQ(sf.values().size(), 1);
  EXPECT_EQ(sf.values().at(0).as<std::string>(), std::string("something"));
}
TEST(step_finder, word_as_number)
{
  step_finder sf("step with 15");
  ASSERT_TRUE(sf.step_matches("step with {word}"));
  EXPECT_EQ(sf.values().size(), 1);
  EXPECT_EQ(sf.values().at(0).as<std::string>(), std::string("15"));
}
TEST(step_finder, word_as_ordinal_number)
{
  step_finder sf("the 5. chapter is cool");
  ASSERT_TRUE(sf.step_matches("the {word} chapter is cool"));
  EXPECT_EQ(sf.values().size(), 1);
  EXPECT_EQ(sf.values().at(0).as<std::string>(), std::string("5."));
}
TEST(step_finder, word_with_non_alpha_chars_1)
{
  step_finder sf("it is -5° outside");
  ASSERT_TRUE(sf.step_matches("step with {word}"));
  EXPECT_EQ(sf.values().size(), 1);
  EXPECT_EQ(sf.values().at(0).as<std::string>(), std::string("-5°"));
}
TEST(step_finder, word_with_non_alpha_chars_2)
{
  step_finder sf("step with a#%@&*");
  ASSERT_TRUE(sf.step_matches("step with {word}"));
  EXPECT_EQ(sf.values().size(), 1);
  EXPECT_EQ(sf.values().at(0).as<std::string>(), std::string("a#%@&*-"));
}
TEST(step_finder, word_between_non_whitespace)
{
  step_finder sf(".hello.");
  ASSERT_TRUE(sf.step_matches(".{word}."));
  EXPECT_EQ(sf.values().size(), 1);
  EXPECT_EQ(sf.values().at(0).as<std::string>(), std::string("hello"));
}
TEST(step_finder, word_followed_by_int)
{
  step_finder sf("two paraemters: {word} {int}");
  ASSERT_TRUE(sf.step_matches("two paraemters: something 11"));
  EXPECT_EQ(sf.values().size(), 2);
  EXPECT_EQ(sf.values().at(0).as<std::string>(), std::string("something"));
  EXPECT_EQ(sf.values().at(0).as<int>(), 11);
}
TEST(step_finder, word_followed_by_int_w_delimiter)
{
  step_finder sf("two paraemters: {word}, {int}");
  ASSERT_TRUE(sf.step_matches("two paraemters: something, 11"));
  EXPECT_EQ(sf.values().size(), 2);
  EXPECT_EQ(sf.values().at(0).as<std::string>(), std::string("something"));
  EXPECT_EQ(sf.values().at(0).as<int>(), 11);
}
