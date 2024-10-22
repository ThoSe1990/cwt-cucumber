#include <gtest/gtest.h>

#include "../src/test_runner.hpp"
#include "../src/parser.hpp"

using namespace cuke::internal;

class run_scenario_hooks : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    before_calls = 0;
    after_calls = 0;
    before_step_calls = 0;
    after_step_calls = 0;

    cuke::registry().clear();

    cuke::registry().push_hook_before(hook([]() { ++before_calls; }));
    cuke::registry().push_hook_after(hook([]() { ++after_calls; }));
    cuke::registry().push_hook_before_step(hook([]() { ++before_step_calls; }));
    cuke::registry().push_hook_after_step(hook([]() { ++after_step_calls; }));

    cuke::registry().push_step(step(
        [](const cuke::value_array&, const auto&, const auto&) {}, "a step"));
  }

  static std::size_t before_calls;
  static std::size_t after_calls;
  static std::size_t before_step_calls;
  static std::size_t after_step_calls;
};
std::size_t run_scenario_hooks::before_calls = 0;
std::size_t run_scenario_hooks::after_calls = 0;
std::size_t run_scenario_hooks::before_step_calls = 0;
std::size_t run_scenario_hooks::after_step_calls = 0;

TEST_F(run_scenario_hooks, run_scenario)
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

  EXPECT_EQ(run_scenario_hooks::before_calls, 1);
  EXPECT_EQ(run_scenario_hooks::after_calls, 1);
  EXPECT_EQ(run_scenario_hooks::before_step_calls, 1);
  EXPECT_EQ(run_scenario_hooks::after_step_calls, 1);
}
TEST_F(run_scenario_hooks, run_scenario_w_tags)
{
  const char* script = R"*(
    Feature: a feature 

    @tag1 
    Scenario: First Scenario 
    Given a step 
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenario_hooks::before_calls, 1);
  EXPECT_EQ(run_scenario_hooks::after_calls, 1);
  EXPECT_EQ(run_scenario_hooks::before_step_calls, 1);
  EXPECT_EQ(run_scenario_hooks::after_step_calls, 1);
}

class run_scenario_hook_skip : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    calls = 0;

    cuke::registry().clear();

    cuke::registry().push_step(step(
        [](const cuke::value_array&, const auto&, const auto&) { ++calls; },
        "a step"));

    cuke::registry().push_step(step(
        [](const cuke::value_array&, const auto&, const auto&) { ++calls; },
        "a step with {int}"));

    cuke::registry().push_hook_before(
        hook([]() { cuke::skip_scenario(); }, "@skip"));
  }
  static std::size_t calls;
};
std::size_t run_scenario_hook_skip::calls = 0;

TEST_F(run_scenario_hook_skip, skip_taggeed_scenario)
{
  const char* script = R"*(
    Feature: a feature 

    @skip
    Scenario: First Scenario 
    Given a step 
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenario_hook_skip::calls, 0);
}
TEST_F(run_scenario_hook_skip, run_tagged_scenario)
{
  const char* script = R"*(
    Feature: a feature 

    @skip
    Scenario: First Scenario 
    Given a step

    Scenario: Second Scenario 
    Given a step
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenario_hook_skip::calls, 1);
}
TEST_F(run_scenario_hook_skip, skip_taggeed_feature)
{
  const char* script = R"*(
    @skip
    Feature: a feature 

    Scenario: First Scenario 
    Given a step 
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenario_hook_skip::calls, 0);
}
TEST_F(run_scenario_hook_skip, skip_taggeed_scenario_outline)
{
  const char* script = R"*(
    Feature: a feature 

    @skip
    Scenario Outline: First Scenario Outline
    Given a step with <value> 
    
    Examples: 
    | value | 
    | 1     |
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenario_hook_skip::calls, 0);
}
TEST_F(run_scenario_hook_skip, run_taggeed_scenario_outline)
{
  const char* script = R"*(
    Feature: a feature 

    @skip
    Scenario Outline: First Scenario Outline
    Given a step with <value> 
    
    Examples: 
    | value | 
    | 1     |

    Scenario Outline: First Scenario Outline
    Given a step with <value> 
    
    Examples: 
    | value | 
    | 1     |
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenario_hook_skip::calls, 1);
}
TEST_F(run_scenario_hook_skip, skip_taggeed_example)
{
  const char* script = R"*(
    Feature: a feature 

    Scenario Outline: First Scenario Outline
    Given a step with <value> 
    
    @skip
    Examples: 
    | value | 
    | 1     |
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenario_hook_skip::calls, 0);
}
TEST_F(run_scenario_hook_skip, run_taggeed_example)
{
  const char* script = R"*(
    Feature: a feature 

    Scenario Outline: First Scenario Outline
    Given a step with <value> 
    
    Examples: 
    | value | 
    | 1     |
    @skip
    Examples: 
    | value | 
    | 1     |
 )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenario_hook_skip::calls, 1);
}
