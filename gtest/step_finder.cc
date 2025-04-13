#include <gtest/gtest.h>

#include "../src/get_args.hpp"
#include "../src/step_finder.hpp"
#include "../src/util_regex.hpp"
#include "registry.hpp"

using namespace cuke::internal;

TEST(step_finder, step_finder)
{
  EXPECT_TRUE(step_finder("foo").step_matches("foo"));
}
TEST(step_finder, only_double_1)
{
  auto [pattern, types] = create_regex_definition("{double}");
  EXPECT_TRUE(step_finder("123.1").step_matches(pattern));
}
TEST(step_finder, only_double_2)
{
  auto [pattern, types] = create_regex_definition("{double}");
  EXPECT_TRUE(step_finder("123.123").step_matches(pattern));
}
TEST(step_finder, only_double_failing)
{
  auto [pattern, types] = create_regex_definition("{double}");
  EXPECT_FALSE(step_finder("hello").step_matches(pattern));
}
TEST(step_finder, step_with_string)
{
  {
    auto [pattern, types] =
        create_regex_definition("a step with a {string} inside");
    EXPECT_TRUE(step_finder("a step with a \"string value\" inside")
                    .step_matches(pattern));
  }
  {
    auto [pattern, types] =
        create_regex_definition("{string} at the beginning");
    EXPECT_TRUE(
        step_finder("\"string value\" at the beginning").step_matches(pattern));
  }
  {
    auto [pattern, types] = create_regex_definition("or a trailing {string}");
    EXPECT_TRUE(
        step_finder("or a trailing \"string value\"").step_matches(pattern));
  }
  {
    auto [pattern, types] = create_regex_definition(
        "{string} at the beginning, {string} inside and a trailing {string}");
    EXPECT_TRUE(
        step_finder("\"string value\" at the beginning, \"string value\" "
                    "inside and a "
                    "trailing \"string value\"")
            .step_matches(pattern));
  }
}
TEST(step_finder, step_with_numbers)
{
  {
    auto [pattern, types] =
        create_regex_definition("a step with a {int} inside");
    EXPECT_TRUE(step_finder("a step with a 123 inside").step_matches(pattern));
  }
  {
    auto [pattern, types] =
        create_regex_definition("a step with a {double} inside");
    EXPECT_TRUE(
        step_finder("a step with a 123.456 inside").step_matches(pattern));
  }
  {
    auto [pattern, types] = create_regex_definition("{int} at the beginning");
    EXPECT_TRUE(step_finder("15 at the beginning").step_matches(pattern));
  }
  {
    auto [pattern, types] = create_regex_definition("or a trailing {double}");
    EXPECT_TRUE(step_finder("or a trailing 55.123").step_matches(pattern));
  }
  {
    auto [pattern, types] = create_regex_definition(
        "{double} at the beginning, {int} inside and a trailing {double}");
    EXPECT_TRUE(
        step_finder("12.12 at the beginning, 1212 inside and a trailing 9999.1")
            .step_matches(pattern));
  }
}
TEST(step_finder, step_with_string_and_int)
{
  auto [pattern, types] =
      create_regex_definition("a step with a {string} and {int} inside");
  EXPECT_TRUE(step_finder("a step with a \"string value\" and 123 inside")
                  .step_matches(pattern));
}
TEST(step_finder, wrong_data_type)
{
  {
    auto [pattern, types] = create_regex_definition("a step with a {string}");
    EXPECT_FALSE(step_finder("a step with a 123").step_matches(pattern));
  }
  {
    auto [pattern, types] = create_regex_definition("a step with a {int}");
    EXPECT_FALSE(
        step_finder("a step with a \"hello world\"").step_matches(pattern));
  }
}
TEST(step_finder, three_ints)
{
  auto [pattern, types] =
      create_regex_definition("A box with {int} x {int} x {int}");
  EXPECT_TRUE(step_finder("A box with 2 x 2 x 2").step_matches(pattern));
}
TEST(step_finder, chinese_letters)
{
  auto [pattern, types] =
      create_regex_definition("一个盒子有 {int} x {int} x {int}");
  EXPECT_TRUE(step_finder("一个盒子有 2 x 2 x 2").step_matches(pattern));
}
TEST(step_finder, step_with_variable)
{
  cuke::value_array data{cuke::value(std::string("value")),
                         cuke::value(std::string("some string value"))};
  cuke::table t(data, 1);

  auto [pattern, types] = create_regex_definition("A Step with a {string}");
  step_finder sf("A Step with a \"<value>\"", t.hash_row(1));
  ASSERT_TRUE(sf.step_matches(pattern));
  ASSERT_EQ(sf.values().size(), 1);
  EXPECT_EQ(sf.values()[0].as<std::string>(), std::string("some string value"));
}
TEST(step_finder, step_with_variables_1)
{
  cuke::value_array data{
      cuke::value(std::string("value")), cuke::value(std::string("integer")),
      cuke::value(std::string("some string value")), cuke::value("12")};
  cuke::table t(data, 2);

  auto [pattern, types] =
      create_regex_definition("A Step with a {string} and {int}");
  step_finder sf("A Step with a \"<value>\" and <integer>", t.hash_row(1));
  ASSERT_TRUE(sf.step_matches(pattern));
  ASSERT_EQ(sf.values().size(), 2);
  EXPECT_EQ(sf.values()[0].as<std::string>(), std::string("some string value"));
  EXPECT_EQ(sf.values()[1].as<int>(), 12);
}
TEST(step_finder, step_with_variables_2)
{
  cuke::value_array data{
      cuke::value(std::string("value")), cuke::value(std::string("integer")),
      cuke::value(std::string("some string value")), cuke::value("12")};
  cuke::table t(data, 2);

  auto [pattern, types] =
      create_regex_definition("A Step with a {string} {string} and {int}");
  step_finder sf("A Step with a \"<value>\" \"<value>\" and <integer>",
                 t.hash_row(1));
  ASSERT_TRUE(sf.step_matches(pattern));
  ASSERT_EQ(sf.values().size(), 3);
  EXPECT_EQ(sf.values()[0].as<std::string>(), std::string("some string value"));
  EXPECT_EQ(sf.values()[1].as<std::string>(), std::string("some string value"));
  EXPECT_EQ(sf.values()[2].as<int>(), 12);
}
TEST(step_finder, value_int)
{
  auto [pattern, types] = create_regex_definition("{int} a step");
  step_finder sf("456 a step");
  EXPECT_TRUE(sf.step_matches(pattern));
  EXPECT_EQ(sf.values().at(0).as<int>(), 456);
}
TEST(step_finder, value_double)
{
  auto [pattern, types] = create_regex_definition("{double} a step");
  step_finder sf("1.123 a step");
  EXPECT_TRUE(sf.step_matches(pattern));
  EXPECT_EQ(sf.values().at(0).as<double>(), 1.123);
}
TEST(step_finder, value_float)
{
  auto [pattern, types] = create_regex_definition("{float} a step");
  step_finder sf("1.1 a step");
  EXPECT_TRUE(sf.step_matches(pattern));
  EXPECT_EQ(sf.values().at(0).as<float>(), 1.1f);
}
TEST(step_finder, value_string)
{
  auto [pattern, types] = create_regex_definition("{string} a step");
  step_finder sf("\"hello world\" a step");
  EXPECT_TRUE(sf.step_matches(pattern));
  EXPECT_EQ(sf.values().at(0).as<std::string>(), std::string("hello world"));
}
TEST(step_finder, multiple_values)
{
  auto [pattern, types] = create_regex_definition(
      "{int} and {double} and {float} and {byte} and {string} and {short}");
  step_finder sf("1 and 2.2 and 3.3 and 4 and \"five\" and 6");
  EXPECT_TRUE(sf.step_matches(pattern));
  EXPECT_EQ(sf.values().at(0).as<int>(), 1);
  EXPECT_EQ(sf.values().at(1).as<double>(), 2.2);
  EXPECT_EQ(sf.values().at(2).as<float>(), 3.3f);
  EXPECT_EQ(sf.values().at(3).as<char>(), 4);
  EXPECT_EQ(sf.values().at(4).as<std::string>(), std::string("five"));
  EXPECT_EQ(sf.values().at(5).as<short>(), 6);
}

TEST(step_finder, word_at_stepend)
{
  auto [pattern, types] = create_regex_definition("step with {word}");
  step_finder sf("step with something");
  ASSERT_TRUE(sf.step_matches(pattern));
  EXPECT_EQ(sf.values().size(), 1);
  EXPECT_EQ(sf.values().at(0).as<std::string>(), std::string("something"));
}
TEST(step_finder, word_at_stepbegin)
{
  auto [pattern, types] = create_regex_definition("{word} in step");
  step_finder sf("something in step");
  ASSERT_TRUE(sf.step_matches(pattern));
  EXPECT_EQ(sf.values().size(), 1);
  EXPECT_EQ(sf.values().at(0).as<std::string>(), std::string("something"));
}
TEST(step_finder, word_as_number)
{
  auto [pattern, types] = create_regex_definition("step with {word}");
  step_finder sf("step with 15");
  ASSERT_TRUE(sf.step_matches(pattern));
  ASSERT_EQ(sf.values().size(), 1);
  EXPECT_EQ(sf.values().at(0).as<std::string>(), std::string("15"));
}
TEST(step_finder, word_as_ordinal_number)
{
  auto [pattern, types] = create_regex_definition("the {word} chapter is cool");
  step_finder sf("the 5. chapter is cool");
  ASSERT_TRUE(sf.step_matches(pattern));
  EXPECT_EQ(sf.values().size(), 1);
  EXPECT_EQ(sf.values().at(0).as<std::string>(), std::string("5."));
}
TEST(step_finder, word_with_non_alpha_chars_1)
{
  auto [pattern, types] = create_regex_definition("it is {word} outside");
  step_finder sf("it is -5° outside");
  ASSERT_TRUE(sf.step_matches(pattern));
  EXPECT_EQ(sf.values().size(), 1);
  EXPECT_EQ(sf.values().at(0).as<std::string>(), std::string("-5°"));
}
TEST(step_finder, word_with_non_alpha_chars_2)
{
  auto [pattern, types] = create_regex_definition("step with {word}");
  step_finder sf("step with {}[]()a#%@&*-");
  ASSERT_TRUE(sf.step_matches(pattern));
  EXPECT_EQ(sf.values().size(), 1);
  EXPECT_EQ(sf.values().at(0).as<std::string>(), std::string("{}[]()a#%@&*-"));
}
TEST(step_finder, word_between_non_whitespace)
{
  auto [pattern, types] = create_regex_definition(".{word}.");
  step_finder sf(".hello.");
  ASSERT_TRUE(sf.step_matches(pattern));
  EXPECT_EQ(sf.values().size(), 1);
  EXPECT_EQ(sf.values().at(0).as<std::string>(), std::string("hello"));
}
TEST(step_finder, word_followed_by_int)
{
  auto [pattern, types] =
      create_regex_definition("two paraemters: {word} {int}");
  step_finder sf("two paraemters: something 11");
  ASSERT_TRUE(sf.step_matches(pattern));
  EXPECT_EQ(sf.values().size(), 2);
  EXPECT_EQ(sf.values().at(0).as<std::string>(), std::string("something"));
  EXPECT_EQ(sf.values().at(1).as<int>(), 11);
}
TEST(step_finder, word_followed_by_int_w_delimiter)
{
  auto [pattern, types] =
      create_regex_definition("two paraemters: {word}, {int}");
  step_finder sf("two paraemters: something, 11");
  ASSERT_TRUE(sf.step_matches(pattern));
  EXPECT_EQ(sf.values().size(), 2);
  EXPECT_EQ(sf.values().at(0).as<std::string>(), std::string("something"));
  EXPECT_EQ(sf.values().at(1).as<int>(), 11);
}
TEST(step_finder, variable_and_word)
{
  cuke::value_array data{cuke::value(std::string("value")),
                         cuke::value(std::string("aWordOr2Here"))};
  cuke::table t(data, 1);

  auto [pattern, types] =
      create_regex_definition("A Step with a {word} and an {int}");
  step_finder sf("A Step with a <value> and an 123", t.hash_row(1));
  ASSERT_TRUE(sf.step_matches(pattern));
  ASSERT_EQ(sf.values().size(), 2);
  EXPECT_EQ(sf.values()[0].as<std::string>(), std::string("aWordOr2Here"));
  EXPECT_EQ(sf.values()[1].as<int>(), 123);
}
TEST(step_finder, variable_and_string)
{
  cuke::value_array data{cuke::value(std::string("value")),
                         cuke::value(std::string("some string value"))};
  cuke::table t(data, 1);

  auto [pattern, types] =
      create_regex_definition("A Step with a {string} and an {int}");
  step_finder sf("A Step with a \"<value>\" and an 123", t.hash_row(1));
  ASSERT_TRUE(sf.step_matches(pattern));
  ASSERT_EQ(sf.values().size(), 2);
  EXPECT_EQ(sf.values()[0].as<std::string>(), std::string("some string value"));
  EXPECT_EQ(sf.values()[1].as<int>(), 123);
}
TEST(step_finder, variable_and_byte)
{
  cuke::value_array data{cuke::value(std::string("value")), cuke::value("99")};
  cuke::table t(data, 1);

  auto [pattern, types] =
      create_regex_definition("A Step with a {byte} and an {int}");
  step_finder sf("A Step with a <value> and an 123", t.hash_row(1));
  ASSERT_TRUE(sf.step_matches(pattern));
  ASSERT_EQ(sf.values().size(), 2);
  EXPECT_EQ(sf.values()[0].as<char>(), 99);
  EXPECT_EQ(sf.values()[1].as<int>(), 123);
}
TEST(step_finder, variable_and_short)
{
  cuke::value_array data{cuke::value(std::string("value")), cuke::value("99")};
  cuke::table t(data, 1);

  step_finder sf("A Step with a <value> and an 123", t.hash_row(1));
  auto [pattern, types] =
      create_regex_definition("A Step with a {short} and an {int}");
  ASSERT_TRUE(sf.step_matches(pattern));
  ASSERT_EQ(sf.values().size(), 2);
  EXPECT_EQ(sf.values()[0].as<char>(), 99);
  EXPECT_EQ(sf.values()[1].as<int>(), 123);
}
TEST(step_finder, variable_and_int)
{
  cuke::value_array data{cuke::value(std::string("value")), cuke::value("99")};
  cuke::table t(data, 1);

  auto [pattern, types] =
      create_regex_definition("A Step with a {int} and an {int}");
  step_finder sf("A Step with a <value> and an 123", t.hash_row(1));
  ASSERT_TRUE(sf.step_matches(pattern));
  ASSERT_EQ(sf.values().size(), 2);
  EXPECT_EQ(sf.values()[0].as<int>(), 99);
  EXPECT_EQ(sf.values()[1].as<int>(), 123);
}
TEST(step_finder, variable_and_long)
{
  cuke::value_array data{cuke::value(std::string("value")), cuke::value("99")};
  cuke::table t(data, 1);

  auto [pattern, types] =
      create_regex_definition("A Step with a {long} and an {int}");
  step_finder sf("A Step with a <value> and an 123", t.hash_row(1));
  ASSERT_TRUE(sf.step_matches(pattern));
  ASSERT_EQ(sf.values().size(), 2);
  EXPECT_EQ(sf.values()[0].as<long>(), 99);
  EXPECT_EQ(sf.values()[1].as<int>(), 123);
}
TEST(step_finder, anonymous_placeholder)
{
  auto [pattern, types] = create_regex_definition("{}");
  EXPECT_TRUE(step_finder("helLO !@#$%^&*())_{}[]").step_matches(pattern));
}
TEST(step_finder, anonymous_in_step_as_number)
{
  auto [pattern, types] = create_regex_definition("step with {}");
  step_finder sf("step with 15");
  ASSERT_TRUE(sf.step_matches(pattern));
  ASSERT_EQ(sf.values().size(), 1);
  EXPECT_EQ(sf.values().at(0).as<std::string>(), std::string("15"));
}
TEST(step_finder, anonymous_in_step_trailing_wo_whitespace)
{
  auto [pattern, types] = create_regex_definition("step with{}");
  step_finder sf("step withtrailingCharsHere");
  ASSERT_TRUE(sf.step_matches(pattern));
  ASSERT_EQ(sf.values().size(), 1);
  EXPECT_EQ(sf.values().at(0).as<std::string>(),
            std::string("trailingCharsHere"));
}
TEST(step_finder, anonymous_in_sequence)
{
  auto [pattern, types] = create_regex_definition("step with {} {}");
  step_finder sf("step with something here and here");
  ASSERT_TRUE(sf.step_matches(pattern));
  ASSERT_EQ(sf.values().size(), 2);
  EXPECT_EQ(sf.values().at(0).as<std::string>(),
            std::string("something here and"));
  EXPECT_EQ(sf.values().at(1).as<std::string>(), std::string("here"));
}
TEST(step_finder, anonymous_in_step_with_variable)
{
  cuke::value_array data{cuke::value(std::string("value")),
                         cuke::value(std::string("99"))};
  cuke::table t(data, 1);

  auto [pattern, types] =
      create_regex_definition("A Step with a {} and an {int}");
  step_finder sf("A Step with a <value> and an 123", t.hash_row(1));
  ASSERT_TRUE(sf.step_matches(pattern));
  ASSERT_EQ(sf.values().size(), 2);
  EXPECT_EQ(sf.values()[0].as<std::string>(), std::string("99"));
  EXPECT_EQ(sf.values()[1].as<int>(), 123);
}
TEST(step_finder, anonymous_between_int_and_word)
{
  auto [pattern, types] = create_regex_definition("step with {int} {} {word}");
  step_finder sf("step with 123 some blalba nonsense !@#$%^&* and a word");
  ASSERT_TRUE(sf.step_matches(pattern));
  ASSERT_EQ(sf.values().size(), 3);
  EXPECT_EQ(sf.values().at(0).as<int>(), 123);
  EXPECT_EQ(sf.values().at(1).as<std::string>(),
            std::string("some blalba nonsense !@#$%^&* and a"));
  EXPECT_EQ(sf.values().at(2).as<std::string>(), std::string("word"));
}
TEST(step_finder, box_examples_1)
{
  auto [pattern, types] =
      create_regex_definition("The box contains {int} items");
  step_finder sf("The box contains 2 items");
  ASSERT_TRUE(sf.step_matches(pattern));
  ASSERT_EQ(sf.values().size(), 1);
  EXPECT_EQ(sf.values().at(0).as<int>(), 2);
}
TEST(step_finder, multiple_strings_1)
{
  auto [pattern, types] =
      create_regex_definition("the program {string} is started");
  step_finder sf(
      "the program \"echo\" with parameters \"should be shown\" is started");
  ASSERT_FALSE(sf.step_matches(pattern));
}
TEST(step_finder, multiple_strings_2)
{
  auto [pattern, types] = create_regex_definition(
      "the program {string} with parameters {string} is started");
  step_finder sf(
      "the program \"echo\" with parameters \"should be shown\" is started");
  ASSERT_TRUE(sf.step_matches(pattern));
  ASSERT_EQ(sf.values().size(), 2);
  EXPECT_EQ(sf.values().at(0).to_string(), std::string("echo"));
  EXPECT_EQ(sf.values().at(1).to_string(), std::string("should be shown"));
}

TEST(step_finder, step_alternation_1)
{
  auto [pattern, types] = create_regex_definition("The book(s) is/are good");
  {
    step_finder sf("The book is good");
    EXPECT_TRUE(sf.step_matches(pattern));
  }
  {  // yes this is weird but valid
    step_finder sf("The books is good");
    EXPECT_TRUE(sf.step_matches(pattern));
  }
  {
    step_finder sf("The books are good");
    EXPECT_TRUE(sf.step_matches(pattern));
  }
}
TEST(step_finder, step_alternation_2)
{
  auto [pattern, types] = create_regex_definition(
      "The book(s) is/are good and has/have {int} reader(s)");
  {
    step_finder sf("The book is good and has 5 readers");
    EXPECT_TRUE(sf.step_matches(pattern));
    ASSERT_EQ(sf.values().size(), 1);
    EXPECT_EQ(sf.values().at(0).as<int>(), 5);
  }
  {
    step_finder sf("The book is good and has 1 reader");
    EXPECT_TRUE(sf.step_matches(pattern));
    ASSERT_EQ(sf.values().size(), 1);
    EXPECT_EQ(sf.values().at(0).as<int>(), 1);
  }
  {
    step_finder sf("The books are good and have 20 readers");
    EXPECT_TRUE(sf.step_matches(pattern));
    ASSERT_EQ(sf.values().size(), 1);
    EXPECT_EQ(sf.values().at(0).as<int>(), 20);
  }
  {
    step_finder sf("The books are good and have 1 reader");
    EXPECT_TRUE(sf.step_matches(pattern));
    ASSERT_EQ(sf.values().size(), 1);
    EXPECT_EQ(sf.values().at(0).as<int>(), 1);
  }
}

class custom_types : public ::testing::Test
{
 protected:
  void SetUp() override { cuke::registry().clear(); }
};

TEST_F(custom_types, custom_conversions_1)
{
  cuke::registry().push_expression(
      "{pair of integers}",
      {R"(var1 = (\d+), var2 = (\d+))", "two integers",
       [](cuke::value_array::const_iterator begin, std::size_t count) -> any
       {
         int var1 = get_param_value(begin, count, 1);
         int var2 = get_param_value(begin, count, 2);
         return std::make_pair(var1, var2);
       }});
  auto [pattern, types] = create_regex_definition("I have {pair of integers}");
  ASSERT_EQ(types.size(), 1);
  EXPECT_EQ(types.at(0).offset, 0);
  EXPECT_EQ(types.at(0).param_count, 2);
  EXPECT_EQ(types.at(0).description, std::string("two integers"));

  step_finder sf("I have var1 = 123, var2 = 999");
  ASSERT_TRUE(sf.step_matches(pattern));
  ASSERT_EQ(sf.values().size(), 2);
  EXPECT_EQ(sf.values().at(0).as<int>(), 123);
  EXPECT_EQ(sf.values().at(1).as<int>(), 999);
}
TEST_F(custom_types, custom_conversions_2)
{
  cuke::registry().push_expression(
      "{pair of integers}",
      {R"(var1 = (\d+), var2 = (\d+))", "two integers",
       [](cuke::value_array::const_iterator begin, std::size_t count) -> any
       {
         int var1 = get_param_value(begin, count, 1);
         int var2 = get_param_value(begin, count, 2);
         return std::make_pair(var1, var2);
       }});
  auto [pattern, types] =
      create_regex_definition("I have {pair of integers} and {int}");
  ASSERT_EQ(types.size(), 2);
  EXPECT_EQ(types.at(1).offset, 1);
  EXPECT_EQ(types.at(1).param_count, 1);
  EXPECT_EQ(types.at(1).description, std::string("int"));

  step_finder sf("I have var1 = 123, var2 = 999 and 5");
  ASSERT_TRUE(sf.step_matches(pattern));
  ASSERT_EQ(sf.values().size(), 3);
  EXPECT_EQ(sf.values().at(0).as<int>(), 123);
  EXPECT_EQ(sf.values().at(1).as<int>(), 999);
  EXPECT_EQ(sf.values().at(2).as<int>(), 5);
}
TEST_F(custom_types, custom_conversions_3)
{
  cuke::registry().push_expression(
      "{pair of integers}",
      {R"(var1 = (\d+), var2 = (\d+))", "two integers",
       [](cuke::value_array::const_iterator begin, std::size_t count) -> any
       {
         int var1 = get_param_value(begin, count, 1);
         int var2 = get_param_value(begin, count, 2);
         return std::make_pair(var1, var2);
       }});
  auto [pattern, types] =
      create_regex_definition("I have {pair of integers} and {int}");
  step_finder sf("I have var1 = 123, var2 = 999 and 5");
  ASSERT_TRUE(sf.step_matches(pattern));

  std::pair<int, int> p = cuke::internal::get_arg(
      sf.values().begin(), sf.values().size(), types, 1, "", 0);
  int i = cuke::internal::get_arg(sf.values().begin(), sf.values().size(),
                                  types, 2, "", 0);

  EXPECT_EQ(p.first, 123);
  EXPECT_EQ(p.second, 999);
  EXPECT_EQ(i, 5);
}
TEST_F(custom_types, custom_conversions_4)
{
  cuke::registry().push_expression(
      "{pair of integers}",
      {R"(var1 = (\d+), var2 = (\d+))", "two integers",
       [](cuke::value_array::const_iterator begin, std::size_t count) -> any
       {
         int var1 = get_param_value(begin, count, 1);
         int var2 = get_param_value(begin, count, 2);
         return std::make_pair(var1, var2);
       }});
  auto [pattern, types] =
      create_regex_definition("I have {} {pair of integers} {} and {int}");
  step_finder sf(
      "I have some text before my type: var1 = 123, var2 = 999 <- and after it "
      "... and 5");
  ASSERT_TRUE(sf.step_matches(pattern));

  std::string anonymous_1 = cuke::internal::get_arg(
      sf.values().begin(), sf.values().size(), types, 1, "", 0);
  ASSERT_EQ(anonymous_1, std::string("some text before my type:"));

  std::pair<int, int> p = cuke::internal::get_arg(
      sf.values().begin(), sf.values().size(), types, 2, "", 0);
  ASSERT_EQ(p.first, 123);
  ASSERT_EQ(p.second, 999);

  std::string anonymous_2 = cuke::internal::get_arg(
      sf.values().begin(), sf.values().size(), types, 3, "", 0);
  ASSERT_EQ(anonymous_2, std::string("<- and after it ..."));

  int i = cuke::internal::get_arg(sf.values().begin(), sf.values().size(),
                                  types, 4, "", 0);
  EXPECT_EQ(i, 5);
}

struct date
{
  int day;
  std::string month;
  int year;
};

struct date_range
{
  date start;
  date end;
};

static cuke::internal::any make_date_range(
    cuke::value_array::const_iterator values_begin, std::size_t count)
{
  date begin;
  begin.month = get_param_value(values_begin, count, 1).as<std::string>();
  begin.day = get_param_value(values_begin, count, 2).as<int>();
  begin.year = get_param_value(values_begin, count, 3).as<int>();

  date end;
  end.month = get_param_value(values_begin, count, 4).as<std::string>();
  end.day = get_param_value(values_begin, count, 5).as<int>();
  end.year = get_param_value(values_begin, count, 6).as<int>();

  return date_range{begin, end};
}

TEST_F(custom_types, custom_conversion_5)
{
  cuke::registry().push_expression(
      "{date}",
      {R"(from ([A-Za-z]+) (\d{1,2}), (\d{4}) to ([A-Za-z]+) (\d{1,2}), (\d{4}))",
       "", make_date_range});
  auto [pattern, types] = create_regex_definition("{} is {date}");
  step_finder sf(
      "The Christmas Market in Augsburg is from November 25, 2024 to December "
      "24, 2024");
  ASSERT_TRUE(sf.step_matches(pattern));
  date_range dr = cuke::internal::get_arg(sf.values().begin(),
                                          sf.values().size(), types, 2, "", 0);
  EXPECT_EQ(dr.start.day, 25);
  EXPECT_EQ(dr.start.month, std::string("November"));
  EXPECT_EQ(dr.start.year, 2024);
  EXPECT_EQ(dr.end.day, 24);
  EXPECT_EQ(dr.end.month, std::string("December"));
  EXPECT_EQ(dr.end.year, 2024);
}
TEST_F(custom_types, custom_conversion_6)
{
  cuke::registry().push_expression(
      "{date}",
      {R"(from ([A-Za-z]+) (\d{1,2}), (\d{4}) to ([A-Za-z]+) (\d{1,2}), (\d{4}))",
       "", make_date_range});
  auto [pattern, types] = create_regex_definition(
      "{} is {date} and a {string} is {date} and {int}");
  step_finder sf(
      "The Christmas Market in Augsburg is from November 25, 2024 to December "
      "24, 2024 and a \"new cool event\" is from January 30, 2025 to February "
      "9, 2025 and 99");
  ASSERT_TRUE(sf.step_matches(pattern));
  {
    date_range dr = cuke::internal::get_arg(
        sf.values().begin(), sf.values().size(), types, 2, "", 0);
    EXPECT_EQ(dr.start.day, 25);
    EXPECT_EQ(dr.start.month, std::string("November"));
    EXPECT_EQ(dr.start.year, 2024);
    EXPECT_EQ(dr.end.day, 24);
    EXPECT_EQ(dr.end.month, std::string("December"));
    EXPECT_EQ(dr.end.year, 2024);
  }
  std::string new_event = cuke::internal::get_arg(
      sf.values().begin(), sf.values().size(), types, 3, "", 0);
  EXPECT_EQ(new_event, std::string("new cool event"));
  {
    date_range dr = cuke::internal::get_arg(
        sf.values().begin(), sf.values().size(), types, 4, "", 0);
    EXPECT_EQ(dr.start.day, 30);
    EXPECT_EQ(dr.start.month, std::string("January"));
    EXPECT_EQ(dr.start.year, 2025);
    EXPECT_EQ(dr.end.day, 9);
    EXPECT_EQ(dr.end.month, std::string("February"));
    EXPECT_EQ(dr.end.year, 2025);
  }
  int number = cuke::internal::get_arg(sf.values().begin(), sf.values().size(),
                                       types, 5, "", 0);
  EXPECT_EQ(number, 99);
}
TEST_F(custom_types, custom_conversions_7)
{
  cuke::registry().push_expression(
      "{another int}",
      {R"(int = (\d+) and int = (\d+))", "",
       [](cuke::value_array::const_iterator begin, std::size_t count) -> any
       {
         int i1 = get_param_value(begin, count, 1);
         int i2 = get_param_value(begin, count, 2);
         return i1 + i2;
       }});
  auto [pattern, types] =
      create_regex_definition("I have {another int} and {int}");
  step_finder sf("I have int = 3 and int = 90 and 5");
  ASSERT_TRUE(sf.step_matches(pattern));

  int i1 = cuke::internal::get_arg(sf.values().begin(), sf.values().size(),
                                   types, 1, "", 0);
  int i2 = cuke::internal::get_arg(sf.values().begin(), sf.values().size(),
                                   types, 2, "", 0);

  EXPECT_EQ(i1, 93);
  EXPECT_EQ(i2, 5);
}
