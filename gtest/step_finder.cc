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
TEST(step_finder, step_with_doc_string_1)
{
  const char* doc_string_step = R"*(this is an arbitrary step with a doc string
"""
this is a doc string
which just follows after the step!
""")*";
  step_finder sf(doc_string_step);
  ASSERT_TRUE(sf.step_matches("this is an arbitrary step with a doc string"));
  EXPECT_EQ(
      sf.values().at(0).as<std::string>(),
      std::string(
          "\nthis is a doc string\nwhich just follows after the step!\n"));
}
TEST(step_finder, step_with_doc_string_2)
{
  const char* doc_string_step = R"*(this is an arbitrary step with a doc string
```
this is a doc string
which just follows after the step!
```)*";
  step_finder sf(doc_string_step);
  ASSERT_TRUE(sf.step_matches("this is an arbitrary step with a doc string"));
  EXPECT_EQ(
      sf.values().at(0).as<std::string>(),
      std::string(
          "\nthis is a doc string\nwhich just follows after the step!\n"));
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
TEST(step_finder, multiple_values_with_doc_string)
{
  const char* doc_string_step = R"*(1 and 2.2 and 3.3 and 4 and "five" and 6
"""
seven
""")*";
  step_finder sf(doc_string_step);
  EXPECT_TRUE(sf.step_matches(
      "{int} and {double} and {float} and {byte} and {string} and {short}"));
  EXPECT_EQ(sf.values().at(6).as<std::string>(), std::string("\nseven\n"));
}

TEST(step_finder, step_w_table_as_const_ref)
{
  const char* doc_string_step = R"*(A datatable:
  | v1 | v2 |
  | 1  | 2  |
  | 3  | 4  |
)*";
  step_finder sf(doc_string_step);
  ASSERT_TRUE(sf.step_matches("A datatable:"));

  const cuke::table& t = *sf.values().at(0).as<table_ptr>().get();
  EXPECT_EQ(t.cells_count(), 6);
  EXPECT_EQ(t.row_count(), 3);
  EXPECT_EQ(t.col_count(), 2);
}

TEST(step_finder, step_w_table_as_copy)
{
  const char* table = R"*(A datatable:
  | v1 | v2 |
  | 1  | 2  |
  | 3  | 4  |
)*";
  step_finder sf(table);
  ASSERT_TRUE(sf.step_matches("A datatable:"));

  cuke::table t = *sf.values().at(0).as<table_ptr>().get();
  EXPECT_EQ(t.cells_count(), 6);
  EXPECT_EQ(t.row_count(), 3);
  EXPECT_EQ(t.col_count(), 2);
}

TEST(step_finder, step_w_strings_in_table_dims)
{
  const char* table = R"*(A datatable:
| NAME              | EMAIL          | CITY      | DOB        |
| Lauriane Mosciski | lm@example.com | Bismarck  | 1954-04-10 |
| Valentin Schultz  | vs@example.com | Lynchburg | 1950-01-01 |
| Shea Ziemann      | sz@example.com | Medford   | 1982-06-19 |
)*";
  step_finder sf(table);
  ASSERT_TRUE(sf.step_matches("A datatable:"));

  cuke::table t = *sf.values().at(0).as<table_ptr>().get();
  EXPECT_EQ(t.cells_count(), 16);
  EXPECT_EQ(t.row_count(), 4);
  EXPECT_EQ(t.col_count(), 4);
}
TEST(step_finder, step_w_strings_in_table_element_type)
{
  const char* table = R"*(A datatable:
| NAME              | EMAIL          | CITY      | DOB        |
| Lauriane Mosciski | lm@example.com | Bismarck  | 1954-04-10 |
| Valentin Schultz  | vs@example.com | Lynchburg | 1950-01-01 |
| Shea Ziemann      | sz@example.com | Medford   | 1982-06-19 |
)*";
  step_finder sf(table);
  ASSERT_TRUE(sf.step_matches("A datatable:"));

  cuke::table t = *sf.values().at(0).as<table_ptr>().get();

  for (std::size_t row = 0; row < 4; ++row)
  {
    for (std::size_t col = 0; col < 4; ++col)
    {
      EXPECT_EQ(t[row][col].type(), cuke::value_type::string);
    }
  }
}

TEST(step_finder, linebreak_eof_after_table)
{
  const char* table = R"*(A datatable:
| NAME              | EMAIL          | CITY      | DOB        |
| Lauriane Mosciski | lm@example.com | Bismarck  | 1954-04-10 |
| Valentin Schultz  | vs@example.com | Lynchburg | 1950-01-01 |
| Shea Ziemann      | sz@example.com | Medford   | 1982-06-19 |
)*";
  step_finder sf(table);
  ASSERT_TRUE(sf.step_matches("A datatable:"));
}
TEST(step_finder, eof_after_table)
{
  const char* table = R"*(A datatable:
| NAME              | EMAIL          | CITY      | DOB        |
| Lauriane Mosciski | lm@example.com | Bismarck  | 1954-04-10 |
| Valentin Schultz  | vs@example.com | Lynchburg | 1950-01-01 |
| Shea Ziemann      | sz@example.com | Medford   | 1982-06-19 |)*";
  step_finder sf(table);
  ASSERT_TRUE(sf.step_matches("A datatable:"));
}
TEST(step_finder, spaces_after_table)
{
  const char* table = R"*(A datatable:
| NAME              | EMAIL          | CITY      | DOB        |
| Lauriane Mosciski | lm@example.com | Bismarck  | 1954-04-10 |
| Valentin Schultz  | vs@example.com | Lynchburg | 1950-01-01 |
| Shea Ziemann      | sz@example.com | Medford   | 1982-06-19 |   )*";
  step_finder sf(table);
  ASSERT_TRUE(sf.step_matches("A datatable:"));
}
