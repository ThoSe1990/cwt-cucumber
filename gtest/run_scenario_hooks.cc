#include <gtest/gtest.h>

#include "../src/test_runner.hpp"
#include "../src/parser.hpp"
#include "../src/options.hpp"
#include "../src/test_results.hpp"
#include "../src/asserts.hpp"

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

    cuke::registry().push_hook_before(hook([]() { ++before_calls; }));
    cuke::registry().push_hook_after(hook([]() { ++after_calls; }));
    cuke::registry().push_hook_before_step(hook([]() { ++before_step_calls; }));
    cuke::registry().push_hook_after_step(hook([]() { ++after_step_calls; }));

    cuke::registry().push_step(step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
        "a step"));
  }

  void TearDown() override
  {
    cuke::registry().clear();
    cuke::results::test_results().clear();
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

    cuke::registry().push_step(step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&)
        { ++calls; }, "a step"));

    cuke::registry().push_step(step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&)
        { ++calls; }, "a step with {int}"));

    cuke::registry().push_hook_before(
        hook([]() { cuke::skip_scenario(); }, "@skip"));
  }

  void TearDown() override
  {
    cuke::registry().clear();
    cuke::results::test_results().clear();
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

class run_scenario_hook_fail_scenario : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    calls = 0;

    cuke::registry().push_step(step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&)
        { ++calls; }, "a step"));

    cuke::registry().push_hook_before(
        hook([]() { cuke::fail_scenario("Scenario Hook: An error occurred"); },
             "@will_fail"));
  }
  void TearDown() override
  {
    cuke::registry().clear();
    cuke::results::test_results().clear();
  }
  static std::size_t calls;
};
std::size_t run_scenario_hook_fail_scenario::calls = 0;

TEST_F(run_scenario_hook_fail_scenario, set_scenario_to_failed_1)
{
  const char* script = R"*(
    Feature: a feature 

    @will_fail
    Scenario: Hook calls cuke::scenario_fail();
    Given a step 
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenario_hook_fail_scenario::calls, 0);
  EXPECT_EQ(cuke::results::test_results().scenarios_passed(), 0);
  EXPECT_EQ(cuke::results::test_results().scenarios_failed(), 1);
  EXPECT_EQ(cuke::results::test_results().steps_skipped(), 1);
}

TEST_F(run_scenario_hook_fail_scenario, set_scenario_to_failed_2)
{
  const char* script = R"*(
    Feature: a feature 

    @will_fail
    Scenario: Hook calls cuke::scenario_fail();
    Given a step 

    Scenario: Will not fail
    Given a step 
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenario_hook_fail_scenario::calls, 1);
  EXPECT_EQ(cuke::results::test_results().scenarios_passed(), 1);
  EXPECT_EQ(cuke::results::test_results().scenarios_failed(), 1);
  EXPECT_EQ(cuke::results::test_results().steps_skipped(), 1);
  EXPECT_EQ(cuke::results::test_results().steps_passed(), 1);
}

class run_scenario_hook_fail_step : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    calls = 0;

    cuke::registry().push_step(step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&)
        { ++calls; }, "a step"));

    cuke::registry().push_hook_before_step(
        hook([]() { cuke::fail_step("Step Hook: An error occurred"); }));
  }
  void TearDown() override
  {
    cuke::registry().clear();
    cuke::results::test_results().clear();
  }
  static std::size_t calls;
};
std::size_t run_scenario_hook_fail_step::calls = 0;

TEST_F(run_scenario_hook_fail_step, set_step_to_failed_1)
{
  const char* script = R"*(
    Feature: a feature 

    Scenario: A Scenario
    Given a step
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenario_hook_fail_step::calls, 0);
  EXPECT_EQ(cuke::results::test_results().scenarios_passed(), 0);
  EXPECT_EQ(cuke::results::test_results().scenarios_failed(), 1);
  EXPECT_EQ(cuke::results::test_results().steps_failed(), 1);
}
TEST_F(run_scenario_hook_fail_step, set_step_to_failed_2)
{
  const char* script = R"*(
    Feature: a feature 

    Scenario: A Scenario
    Given a step
    Scenario: A Scenario
    Given a step
    And a step
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenario_hook_fail_step::calls, 0);
  EXPECT_EQ(cuke::results::test_results().scenarios_passed(), 0);
  EXPECT_EQ(cuke::results::test_results().scenarios_failed(), 2);
  EXPECT_EQ(cuke::results::test_results().steps_failed(), 2);
  EXPECT_EQ(cuke::results::test_results().steps_skipped(), 1);
}
TEST_F(run_scenario_hook_fail_step, set_step_to_failed_3)
{
  const char* script = R"*(
    Feature: a feature 

    Scenario: A Scenario
    Given a step
    Scenario: A Scenario
    Given a step
    And a step
  )*";

  const char* argv[] = {"program", "-c"};
  int argc = sizeof(argv) / sizeof(argv[0]);
  [[maybe_unused]] auto& args = cuke::internal::get_program_args(argc, argv);

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenario_hook_fail_step::calls, 0);
  EXPECT_EQ(cuke::results::test_results().scenarios_passed(), 0);
  EXPECT_EQ(cuke::results::test_results().scenarios_failed(), 2);
  EXPECT_EQ(cuke::results::test_results().steps_failed(), 3);
}

class run_scenario_hook_asserts : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    calls = 0;

    cuke::registry().push_step(step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&)
        { ++calls; }, "a step"));
  }
  void TearDown() override
  {
    cuke::registry().clear();
    cuke::results::test_results().clear();
  }
  static std::size_t calls;
};
std::size_t run_scenario_hook_asserts::calls = 0;

TEST_F(run_scenario_hook_asserts, assert_in_before_hook_fails_scenario)
{
  cuke::registry().push_hook_before(
      hook([]() { cuke::equal(1, 2); }, "@assert_fails"));

  const char* script = R"*(
    Feature: a feature

    @assert_fails
    Scenario: Before hook assertion fails
    Given a step
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenario_hook_asserts::calls, 0);
  EXPECT_EQ(cuke::results::test_results().scenarios_passed(), 0);
  EXPECT_EQ(cuke::results::test_results().scenarios_failed(), 1);
  EXPECT_EQ(cuke::results::test_results().steps_skipped(), 1);

  const auto& scenarios = cuke::results::test_results().back().scenarios;
  ASSERT_EQ(scenarios.size(), 1);
  EXPECT_EQ(scenarios.at(0).status, cuke::results::test_status::failed);
  ASSERT_EQ(scenarios.at(0).steps.size(), 1);
  EXPECT_EQ(scenarios.at(0).steps.at(0).status,
            cuke::results::test_status::skipped);
}

TEST_F(run_scenario_hook_asserts,
       assert_in_after_hook_fails_scenario_without_affecting_step)
{
  cuke::registry().push_hook_after(
      hook([]() { cuke::equal(1, 2); }, "@assert_fails"));

  const char* script = R"*(
    Feature: a feature

    @assert_fails
    Scenario: After hook assertion fails
    Given a step
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenario_hook_asserts::calls, 1);
  EXPECT_EQ(cuke::results::test_results().scenarios_passed(), 0);
  EXPECT_EQ(cuke::results::test_results().scenarios_failed(), 1);
  EXPECT_EQ(cuke::results::test_results().steps_passed(), 1);

  const auto& scenarios = cuke::results::test_results().back().scenarios;
  ASSERT_EQ(scenarios.size(), 1);
  EXPECT_EQ(scenarios.at(0).status, cuke::results::test_status::failed);
  ASSERT_EQ(scenarios.at(0).steps.size(), 1);
  EXPECT_EQ(scenarios.at(0).steps.at(0).status,
            cuke::results::test_status::passed);
  EXPECT_TRUE(scenarios.at(0).steps.at(0).error_msg.empty());
}

TEST_F(run_scenario_hook_asserts,
       assert_passing_in_hooks_allows_scenario_to_pass)
{
  cuke::registry().push_hook_before(hook(
      []()
      {
        cuke::equal(1, 1);
        cuke::is_true(true);
      },
      "@assert_ok"));
  cuke::registry().push_hook_after(hook(
      []()
      {
        cuke::equal(2, 2);
        cuke::is_false(false);
      },
      "@assert_ok"));

  const char* script = R"*(
    Feature: a feature

    @assert_ok
    Scenario: Hooks with passing assertions
    Given a step
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenario_hook_asserts::calls, 1);
  EXPECT_EQ(cuke::results::test_results().scenarios_passed(), 1);
  EXPECT_EQ(cuke::results::test_results().scenarios_failed(), 0);
  EXPECT_EQ(cuke::results::test_results().steps_passed(), 1);
}

TEST_F(run_scenario_hook_asserts, assert_in_after_hook_preserves_multiple_steps)
{
  cuke::registry().push_hook_after(
      hook([]() { cuke::is_true(false); }, "@assert_fails"));

  const char* script = R"*(
    Feature: a feature

    @assert_fails
    Scenario: Multiple steps before failing after hook
    Given a step
    And a step
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenario_hook_asserts::calls, 2);
  EXPECT_EQ(cuke::results::test_results().scenarios_passed(), 0);
  EXPECT_EQ(cuke::results::test_results().scenarios_failed(), 1);
  EXPECT_EQ(cuke::results::test_results().steps_passed(), 2);

  const auto& scenarios = cuke::results::test_results().back().scenarios;
  ASSERT_EQ(scenarios.size(), 1);
  EXPECT_EQ(scenarios.at(0).status, cuke::results::test_status::failed);
  ASSERT_EQ(scenarios.at(0).steps.size(), 2);
  EXPECT_EQ(scenarios.at(0).steps.at(0).status,
            cuke::results::test_status::passed);
  EXPECT_TRUE(scenarios.at(0).steps.at(0).error_msg.empty());
  EXPECT_EQ(scenarios.at(0).steps.at(1).status,
            cuke::results::test_status::passed);
  EXPECT_TRUE(scenarios.at(0).steps.at(1).error_msg.empty());
}

TEST_F(run_scenario_hook_asserts,
       step_fails_and_after_hook_assert_fails_preserves_step_error)
{
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&)
      { cuke::equal(10, 20); }, "a failing step"));

  cuke::registry().push_hook_after(
      hook([]() { cuke::is_true(false); }, "@assert_fails"));

  const char* script = R"*(
    Feature: a feature

    @assert_fails
    Scenario: Step fails and after hook fails
    Given a failing step
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(cuke::results::test_results().scenarios_passed(), 0);
  EXPECT_EQ(cuke::results::test_results().scenarios_failed(), 1);
  EXPECT_EQ(cuke::results::test_results().steps_failed(), 1);

  const auto& scenarios = cuke::results::test_results().back().scenarios;
  ASSERT_EQ(scenarios.size(), 1);
  EXPECT_EQ(scenarios.at(0).status, cuke::results::test_status::failed);
  ASSERT_EQ(scenarios.at(0).steps.size(), 1);
  EXPECT_EQ(scenarios.at(0).steps.at(0).status,
            cuke::results::test_status::failed);
  EXPECT_EQ(scenarios.at(0).steps.at(0).error_msg,
            std::string("Value 10 is not equal to 20"));
}

TEST_F(run_scenario_hook_asserts, assert_in_before_step_hook_fails_active_step)
{
  cuke::registry().push_hook_before_step(
      hook([]() { cuke::equal(1, 2); }, "@step_hook_assert"));

  const char* script = R"*(
    Feature: a feature

    @step_hook_assert
    Scenario: Before step hook assert fails
    Given a step
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenario_hook_asserts::calls, 0);
  EXPECT_EQ(cuke::results::test_results().scenarios_passed(), 0);
  EXPECT_EQ(cuke::results::test_results().scenarios_failed(), 1);
  EXPECT_EQ(cuke::results::test_results().steps_failed(), 1);

  const auto& scenarios = cuke::results::test_results().back().scenarios;
  ASSERT_EQ(scenarios.size(), 1);
  EXPECT_EQ(scenarios.at(0).status, cuke::results::test_status::failed);
  ASSERT_EQ(scenarios.at(0).steps.size(), 1);
  EXPECT_EQ(scenarios.at(0).steps.at(0).status,
            cuke::results::test_status::failed);
  EXPECT_EQ(scenarios.at(0).steps.at(0).error_msg,
            std::string("Value 1 is not equal to 2"));
}

TEST_F(run_scenario_hook_asserts, assert_in_after_step_hook_fails_active_step)
{
  cuke::registry().push_hook_after_step(
      hook([]() { cuke::equal(1, 2); }, "@step_hook_assert"));

  const char* script = R"*(
    Feature: a feature

    @step_hook_assert
    Scenario: After step hook assert fails
    Given a step
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenario_hook_asserts::calls, 1);
  EXPECT_EQ(cuke::results::test_results().scenarios_passed(), 0);
  EXPECT_EQ(cuke::results::test_results().scenarios_failed(), 1);
  EXPECT_EQ(cuke::results::test_results().steps_failed(), 1);

  const auto& scenarios = cuke::results::test_results().back().scenarios;
  ASSERT_EQ(scenarios.size(), 1);
  EXPECT_EQ(scenarios.at(0).status, cuke::results::test_status::failed);
  ASSERT_EQ(scenarios.at(0).steps.size(), 1);
  EXPECT_EQ(scenarios.at(0).steps.at(0).status,
            cuke::results::test_status::failed);
  EXPECT_EQ(scenarios.at(0).steps.at(0).error_msg,
            std::string("Value 1 is not equal to 2"));
}
