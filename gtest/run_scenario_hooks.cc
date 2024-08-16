#include <gtest/gtest.h>

#include "../src/test_runner.hpp"
#include "../src/parser.hpp"

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

    cuke::registry().push_hook_before(
        cuke::internal::hook([]() { ++before_calls; }));
    cuke::registry().push_hook_after(
        cuke::internal::hook([]() { ++after_calls; }));
    cuke::registry().push_hook_before_step(
        cuke::internal::hook([]() { ++before_step_calls; }));
    cuke::registry().push_hook_after_step(
        cuke::internal::hook([]() { ++after_step_calls; }));

    cuke::registry().push_step(
        cuke::internal::step([](const cuke::value_array&) {}, "a step"));
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
