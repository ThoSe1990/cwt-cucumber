#include <gtest/gtest.h>

#include "../src/ast/parser.hpp"
#include "../src/ast/test_runner.hpp"
#include "../src/ast/test_results.hpp"
#include "../src/asserts.hpp"

class test_results_1 : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    cuke::results::test_results().clear();
    cuke::registry().clear();
    cuke::registry().push_step(
        cuke::internal::step([](const cuke::value_array&) {}, "a step"));
    cuke::registry().push_step(cuke::internal::step(
        [](const cuke::value_array&) { cuke::is_true(false); }, "this fails"));
  }
};

TEST_F(test_results_1, scenario_passed)
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

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  ASSERT_EQ(cuke::results::features_back().scenarios.size(), 1);
  ASSERT_EQ(cuke::results::scenarios_back().steps.size(), 1);
  EXPECT_EQ(cuke::results::features_back().status,
            cuke::results::test_status::passed);
  EXPECT_EQ(cuke::results::scenarios_back().status,
            cuke::results::test_status::passed);
  EXPECT_EQ(cuke::results::steps_back().status,
            cuke::results::test_status::passed);
}

TEST_F(test_results_1, scenario_fails_1)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given this fails 
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  EXPECT_EQ(cuke::results::features_back().status,
            cuke::results::test_status::failed);
  EXPECT_EQ(cuke::results::scenarios_back().status,
            cuke::results::test_status::failed);
  EXPECT_EQ(cuke::results::steps_back().status,
            cuke::results::test_status::failed);
}
TEST_F(test_results_1, scenario_fails_2)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given this fails 
    And this is skipped
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  EXPECT_EQ(cuke::results::features_back().status,
            cuke::results::test_status::failed);
  EXPECT_EQ(cuke::results::scenarios_back().status,
            cuke::results::test_status::failed);
  EXPECT_EQ(cuke::results::steps_back().status,
            cuke::results::test_status::skipped);
}

TEST_F(test_results_1, undefined_step_1)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given a step 
    And an undefined step ... 
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  EXPECT_EQ(cuke::results::features_back().status,
            cuke::results::test_status::failed);
  EXPECT_EQ(cuke::results::scenarios_back().status,
            cuke::results::test_status::failed);
  EXPECT_EQ(cuke::results::steps_back().status,
            cuke::results::test_status::undefined);
}
TEST_F(test_results_1, undefined_step_2)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given this fails 
    And an undefined step ...
    Then this is skipped
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  EXPECT_EQ(cuke::results::features_back().status,
            cuke::results::test_status::failed);
  EXPECT_EQ(cuke::results::scenarios_back().status,
            cuke::results::test_status::failed);
  EXPECT_EQ(cuke::results::steps_back().status,
            cuke::results::test_status::skipped);
}
