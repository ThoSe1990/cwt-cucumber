#include <gtest/gtest.h>
#include <cstddef>

#include "../src/ast/test_runner.hpp"
#include "../src/ast/parser.hpp"

class run_scenarios_1 : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    call_count = 0;
    cuke::registry::clear();
    cuke::registry::push_step(cuke::internal::step(
        [](cuke::value_array::const_iterator, std::size_t) { run_scenarios_1::call_count++; },
        "a step"));
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
  cuke::internal::test_runner runner;
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
  cuke::internal::test_runner runner;
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
  cuke::internal::test_runner runner;
  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_1::call_count, 8);
}

class run_scenarios_2 : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    expected_int = 0;
    expected_string = "";
    cuke::registry::clear();
    cuke::registry::push_step(cuke::internal::step(
        [](cuke::value_array::const_iterator it, std::size_t n)
        {
          run_scenarios_2::expected_int = it->as<unsigned int>();
          run_scenarios_2::expected_string = (it+1)->as<std::string>();
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
  cuke::internal::test_runner runner;
  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_2::expected_int, 5);
  EXPECT_EQ(run_scenarios_2::expected_string, std::string("hello world"));
}
