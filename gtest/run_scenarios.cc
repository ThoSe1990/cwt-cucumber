#include <gtest/gtest.h>

#include "../src/test_runner.hpp"
#include "../src/parser.hpp"

class run_scenarios_1 : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    call_count = 0;
    cuke::registry().clear();
    cuke::registry().push_step(cuke::internal::step(
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
    cuke::registry().push_step(cuke::internal::step(
        [](const cuke::value_array& values, const auto&, const auto&, const auto&)
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
    cuke::registry().push_step(cuke::internal::step(
        [](const cuke::value_array& values, const auto&, const auto&, const auto&)
        { ++call_count; }, "a step with {int}"));
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
    cuke::registry().push_step(cuke::internal::step(
        [](const cuke::value_array& values, const auto&, const auto&, const auto&)
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
