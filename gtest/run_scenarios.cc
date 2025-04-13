#include <gtest/gtest.h>
#include <stdexcept>

#include "../src/test_runner.hpp"
#include "../src/parser.hpp"
#include "table.hpp"

class run_scenarios_1 : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    call_count = 0;
    cuke::registry().clear();
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&)
        { run_scenarios_1::call_count++; }, "a step"));
  }

  static std::size_t call_count;
};
std::size_t run_scenarios_1::call_count = 0;

TEST_F(run_scenarios_1, run_scenario)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given a step 
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_1::call_count, 1);
}
TEST_F(run_scenarios_1, run_scenario_w_multiple_steps)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given a step 
    Given a step 
    Given a step 
    Given a step 
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_1::call_count, 4);
}
TEST_F(run_scenarios_1, run_multiple_scenarios)
{
  const char* script = R"*(
    Feature: a feature 

    Scenario: First Scenario 
    Given a step 

    Scenario: Second Scenario
    Given a step 
    When a step 
    And a step 
    Then a step 

    Scenario: Third
    * a step 
    * a step 
    * a step 
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_1::call_count, 8);
}
TEST_F(run_scenarios_1, background)
{
  const char* script = R"*(
    Feature: a feature 
    Background: 
    Given a step 
    Given a step 

    Scenario: First Scenario 
    Given a step 
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_1::call_count, 3);
}
class run_scenarios_2 : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    expected_int = 0;
    expected_string = "";
    cuke::registry().clear();
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array& values, const auto&, const auto&,
           const auto&)
        {
          run_scenarios_2::expected_int = values[0].as<unsigned int>();
          run_scenarios_2::expected_string = values[1].as<std::string>();
        },
        "a step with {int} and {string}"));
  }
  static std::size_t expected_int;
  static std::string expected_string;
};
std::size_t run_scenarios_2::expected_int = 0;
std::string run_scenarios_2::expected_string = "";

TEST_F(run_scenarios_2, run_scenario)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given a step with 5 and "hello world"
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_2::expected_int, 5);
  EXPECT_EQ(run_scenarios_2::expected_string, std::string("hello world"));
}
TEST_F(run_scenarios_2, run_scenario_outline)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario Outline: First Scenario 
    Given a step with <var 1> and <var 2> 

    Examples: 
      | var 1 | var 2              | 
      | 123  |asdfasdf asdfasdf afsd | 
      | 99    | "scenario outline" | 
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_2::expected_int, 99);
  EXPECT_EQ(run_scenarios_2::expected_string, std::string("scenario outline"));
}
class run_scenarios_3 : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    call_count = 0;
    cuke::registry().clear();
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array& values, const auto&, const auto&,
           const auto&) { ++call_count; },
        "a step with {int}"));
  }
  static std::size_t call_count;
};
std::size_t run_scenarios_3::call_count = 0;

TEST_F(run_scenarios_3, scenario_outline_w_background)
{
  const char* script = R"*(
    Feature: a feature 

    Background:
    Given a step with 1 
    And a step with 2

    Scenario Outline: First Scenario 
    Given a step with <var> 

    Examples: 
      | var | 
      | 1   | 
      | 2   | 
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);
  EXPECT_EQ(call_count, 6);
}

class run_scenarios_4 : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    word_value = "";
    anonymous_value = "";
    cuke::registry().clear();
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array& values, const auto&, const auto&,
           const auto&)
        {
          ASSERT_EQ(values.size(), 2);
          word_value = values.at(0).as<std::string>();
          anonymous_value = values.at(1).as<std::string>();
        },
        "a step with {word} and {}"));
  }
  static std::string word_value;
  static std::string anonymous_value;
};
std::string run_scenarios_4::word_value = "";
std::string run_scenarios_4::anonymous_value = "";

TEST_F(run_scenarios_4, integer_values_as_string)
{
  const char* script = R"*(
    Feature: a feature 

    Scenario Outline: First Scenario 
    Given a step with <word> and <anonymous> 
    Examples: 
      | word | anonymous |
      | 123  | 999       |
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);
  EXPECT_EQ(word_value, "123");
  EXPECT_EQ(anonymous_value, "999");
}
TEST_F(run_scenarios_4, negative_integers_as_string)
{
  const char* script = R"*(
    Feature: a feature 

    Scenario Outline: First Scenario 
    Given a step with <word> and <anonymous> 
    Examples: 
      | word | anonymous |
      | -123 | -999      |
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);
  EXPECT_EQ(word_value, "-123");
  EXPECT_EQ(anonymous_value, "-999");
}
TEST_F(run_scenarios_4, floating_points_as_string)
{
  const char* script = R"*(
    Feature: a feature 

    Scenario Outline: First Scenario 
    Given a step with <word> and <anonymous> 
    Examples: 
      | word | anonymous |
      | 3.12 | -999.99   |
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(word_value, "3.12");
  EXPECT_EQ(anonymous_value, "-999.99");
}

class run_scenarios_5 : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    expected_int = 0;
    cuke::registry().clear();
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array& values, const auto&, const auto&,
           const auto& table)
        {
          run_scenarios_5::expected_int = values[0].as<unsigned int>();
          run_scenarios_5::expected_table = table;
        },
        "a step with {int} and a table"));
  }
  static std::size_t expected_int;
  static cuke::table expected_table;
};
std::size_t run_scenarios_5::expected_int = 0;
cuke::table run_scenarios_5::expected_table{};

TEST_F(run_scenarios_5, data_table_w_vars_1)
{
  const char* script = R"*(
    Feature: a feature 

    Scenario Outline: First Scenario 
    Given a step with <val> and a table 
    | "col" | <table val> |

    Examples: 
      | val | table val |
      | 5   | 999       |
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenarios_5::expected_int, 5);
  ASSERT_EQ(run_scenarios_5::expected_table.cells_count(), 2);
  ASSERT_EQ(run_scenarios_5::expected_table[0][0].to_string(),
            std::string("col"));
  ASSERT_EQ(run_scenarios_5::expected_table[0][1].as<int>(), 999);
}
TEST_F(run_scenarios_5, data_table_w_vars_2)
{
  const char* script = R"*(
    Feature: a feature 

    Scenario Outline: First Scenario 
    Given a step with <value 1> and a table 
    | 1         | 1         |
    | <value 1> | <value 2> |

    Examples: 
      | value 1 |  value 2  |
      | 1       | 2         |
      | 3       | 4         |
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenarios_5::expected_int, 3);
  ASSERT_EQ(run_scenarios_5::expected_table.cells_count(), 4);
  ASSERT_EQ(run_scenarios_5::expected_table[0][0].as<int>(), 1);
  ASSERT_EQ(run_scenarios_5::expected_table[0][1].as<int>(), 1);
  ASSERT_EQ(run_scenarios_5::expected_table[1][0].as<int>(), 3);
  ASSERT_EQ(run_scenarios_5::expected_table[1][1].as<int>(), 4);
}
TEST_F(run_scenarios_5, data_table_w_vars_3)
{
  const char* script = R"*(
    Feature: a feature 

    Scenario Outline: First Scenario 
    Given a step with <value 1> and a table 
    | 1           | 1           |
    | 9<value 1>9 | 9<value 2>9 |

    Examples: 
      | value 1 |  value 2  |
      | 9       | 0         |
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenarios_5::expected_int, 9);
  ASSERT_EQ(run_scenarios_5::expected_table.cells_count(), 4);
  ASSERT_EQ(run_scenarios_5::expected_table[0][0].as<int>(), 1);
  ASSERT_EQ(run_scenarios_5::expected_table[0][1].as<int>(), 1);
  ASSERT_EQ(run_scenarios_5::expected_table[1][0].as<int>(), 999);
  ASSERT_EQ(run_scenarios_5::expected_table[1][1].as<int>(), 909);
}
TEST_F(run_scenarios_5, data_table_w_vars_4)
{
  const char* script = R"*(
    Feature: a feature 

    Scenario Outline: First Scenario 
    Given a step with 123 and a table 
    | just <value 1> here | "a string <value 2> ..." |

    Examples: 
      | value 1   |  value 2   |
      | some text | with a var |
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenarios_5::expected_int, 123);
  ASSERT_EQ(run_scenarios_5::expected_table.cells_count(), 2);
  ASSERT_EQ(run_scenarios_5::expected_table[0][0].to_string(),
            std::string("just some text here"));
  ASSERT_EQ(run_scenarios_5::expected_table[0][1].to_string(),
            std::string("a string with a var ..."));
}
TEST_F(run_scenarios_5, data_table_w_vars_5)
{
  const char* script = R"*(
    Feature: a feature 

    Scenario Outline: First Scenario 
    Given a step with 123 and a table 
    | \<thats not a variable | thats also not a variable \> | but <var> is a variable | 

    Examples: 
      | var  |
      | here |
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenarios_5::expected_int, 123);
  ASSERT_EQ(run_scenarios_5::expected_table.cells_count(), 3);
  ASSERT_EQ(run_scenarios_5::expected_table[0][0].to_string(),
            std::string("\\<thats not a variable"));
  ASSERT_EQ(run_scenarios_5::expected_table[0][1].to_string(),
            std::string("thats also not a variable \\>"));
  ASSERT_EQ(run_scenarios_5::expected_table[0][2].to_string(),
            std::string("but here is a variable"));
}
TEST_F(run_scenarios_5, data_table_w_vars_6)
{
  const char* script = R"*(
    Feature: a feature 

    Scenario Outline: First Scenario 
    Given a step with 123 and a table 
    | <value 1> and <value 2> and <value 3> | and some arbitrary text |

    Examples: 
      | value 1 | value 2 | value 3        |
      | 101     | 102     | something else |
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenarios_5::expected_int, 123);
  ASSERT_EQ(run_scenarios_5::expected_table.cells_count(), 2);
  ASSERT_EQ(run_scenarios_5::expected_table[0][0].to_string(),
            std::string("101 and 102 and something else"));
  ASSERT_EQ(run_scenarios_5::expected_table[0][1].to_string(),
            std::string("and some arbitrary text"));
}
TEST_F(run_scenarios_5, data_table_w_vars_key_doesnt_exist)
{
  const char* script = R"*(
    Feature: a feature 

    Scenario Outline: First Scenario 
    Given a step with <value 1> and a table 
    | 1         | 1              |
    | <value 1> | <non existing> |

    Examples: 
      | value 1 |  value 2  |
      | 1       | 2         |
      | 3       | 4         |
  )*";

  cuke::parser p;
  EXPECT_THROW({ p.parse_script(script); }, std::runtime_error);
}

class run_scenarios_6 : public ::testing::Test
{
  // Reported bug: closing quote was ignored with multiple string parameters
  // within one step: https://github.com/ThoSe1990/cwt-cucumber/issues/80
 protected:
  void SetUp() override
  {
    param1 = std::string{""};
    param2 = std::string{""};
    cuke::registry().clear();
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array& values, const auto&, const auto&,
           const auto&)
        {
          ASSERT_EQ(values.size(), 1);
          param1 = values.at(0).to_string();
          param2 = std::string{""};
        },
        "the program {string} is started"));
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array& values, const auto&, const auto&,
           const auto&)
        {
          ASSERT_EQ(values.size(), 2);
          param1 = values.at(0).to_string();
          param2 = values.at(1).to_string();
        },
        "the program {string} with parameters {string} is started"));
  }
  static std::string param1;
  static std::string param2;
};
std::string run_scenarios_6::param1 = std::string{""};
std::string run_scenarios_6::param2 = std::string{""};

TEST_F(run_scenarios_6, match_step_with_1_arg)
{
  const char* script = R"*(
    Feature: a feature 

    Scenario: 
    * the program "./my-program" is started

  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenarios_6::param1, std::string("./my-program"));
  EXPECT_TRUE(run_scenarios_6::param2.empty());
}
TEST_F(run_scenarios_6, match_step_with_2_arg)
{
  const char* script = R"*(
    Feature: a feature 

    Scenario: 
    * the program "./another-program" with parameters "--arg 1 --arg 'some value'" is started

  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenarios_6::param1, std::string("./another-program"));
  EXPECT_EQ(run_scenarios_6::param2, std::string("--arg 1 --arg 'some value'"));
}
