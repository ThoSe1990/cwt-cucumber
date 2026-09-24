#include <gtest/gtest.h>

#include "../src/test_runner.hpp"
#include "../src/parser.hpp"
#include "../src/asserts.hpp"
#include "../src/test_results.hpp"

class hooks_before : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    hook_called = false;
    step_called = false;
    cuke::registry().clear();

    cuke::registry().push_hook_before(
        cuke::internal::hook([]() { hook_called = true; }));

    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&)
        {
          ASSERT_TRUE(hook_called);
          step_called = true;
        },
        "a step"));
  }
  static bool step_called;
  static bool hook_called;
};
bool hooks_before::step_called = false;
bool hooks_before::hook_called = false;

TEST_F(hooks_before, run_scenario)
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
  EXPECT_TRUE(hooks_before::hook_called);
  EXPECT_TRUE(hooks_before::step_called);
}

class hooks_after : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    hook_called = false;
    step_called = false;
    cuke::registry().clear();

    cuke::registry().push_hook_after(
        cuke::internal::hook([]() { hook_called = true; }));

    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&)
        {
          ASSERT_FALSE(hook_called);
          step_called = true;
        },
        "a step"));
  }
  static bool step_called;
  static bool hook_called;
};
bool hooks_after::step_called = false;
bool hooks_after::hook_called = false;

TEST_F(hooks_after, run_scenario)
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
  EXPECT_TRUE(hooks_after::hook_called);
  EXPECT_TRUE(hooks_after::step_called);
}
class hooks_after_tagged : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    hook_called = false;
    step_called = false;
    cuke::registry().clear();

    cuke::registry().push_hook_after(
        cuke::internal::hook([]() { hook_called = true; }, "@tag"));

    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&)
        {
          ASSERT_FALSE(hook_called);
          step_called = true;
        },
        "a step"));
  }
  static bool step_called;
  static bool hook_called;
};
bool hooks_after_tagged::step_called = false;
bool hooks_after_tagged::hook_called = false;

TEST_F(hooks_after_tagged, ignore_hook)
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
  EXPECT_FALSE(hooks_after_tagged::hook_called);
  EXPECT_TRUE(hooks_after_tagged::step_called);
}
TEST_F(hooks_after_tagged, execute_hook)
{
  const char* script = R"*(
    Feature: a feature 

    @tag
    Scenario: First Scenario 
    Given a step 
  )*";

  cuke::parser p;
  p.parse_script(script);

  cuke::test_runner runner;
  p.for_each_scenario(runner);
  EXPECT_TRUE(hooks_after_tagged::hook_called);
  EXPECT_TRUE(hooks_after_tagged::step_called);
}

class hooks_before_all : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    hook_called = false;
    step_called = false;
    cuke::registry().clear();
    cuke::results::test_results().clear();

    cuke::registry().push_hook_before_all(
        cuke::internal::hook([]() { hook_called = true; }));

    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&)
        {
          ASSERT_TRUE(hook_called);
          step_called = true;
        },
        "a step"));
  }

  void TearDown() override
  {
    cuke::registry().clear();
    cuke::results::test_results().clear();
  }

  static bool step_called;
  static bool hook_called;
  static bool step1_called;
  static bool step2_called;
};
bool hooks_before_all::step_called = false;
bool hooks_before_all::hook_called = false;
bool hooks_before_all::step1_called = false;
bool hooks_before_all::step2_called = false;

TEST_F(hooks_before_all, run_scenario)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given a step 
  )*";

  cuke::parser p;
  p.parse_script(script);

  cuke::test_runner runner;
  runner.setup();
  p.for_each_scenario(runner);
  runner.teardown();

  EXPECT_TRUE(hooks_before_all::hook_called);
  EXPECT_TRUE(hooks_before_all::step_called);
}

TEST_F(hooks_before_all,
       failing_assert_in_before_all_fails_run_and_skips_scenarios)
{
  cuke::registry().clear();
  cuke::results::test_results().clear();

  cuke::registry().push_hook_before_all(
      cuke::internal::hook([]() { cuke::equal(1, 2); }));

  step1_called = false;
  step2_called = false;
  cuke::registry().push_step(cuke::internal::step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&)
      { step1_called = true; }, "first step"));
  cuke::registry().push_step(cuke::internal::step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&)
      { step2_called = true; }, "second step"));

  const char* script = R"*(
    Feature: a feature
    Scenario: First Scenario
    Given first step
    Scenario: Second Scenario
    Given second step
  )*";

  cuke::parser p;
  p.parse_script(script);

  cuke::test_runner runner;
  runner.setup();
  p.for_each_scenario(runner);
  runner.teardown();

  EXPECT_FALSE(step1_called);
  EXPECT_FALSE(step2_called);
  EXPECT_EQ(cuke::results::test_results().scenarios_skipped(), 2);
  EXPECT_EQ(cuke::results::final_result(), cuke::results::test_status::failed);
}

TEST_F(hooks_before_all,
       failing_assert_in_before_all_continues_when_flag_is_set)
{
  const char* argv[] = {"program", "--continue-on-failure"};
  int argc = sizeof(argv) / sizeof(argv[0]);
  [[maybe_unused]] auto& args = cuke::internal::get_program_args(argc, argv);

  cuke::registry().clear();
  cuke::results::test_results().clear();

  cuke::registry().push_hook_before_all(
      cuke::internal::hook([]() { cuke::equal(1, 2); }));

  step1_called = false;
  step2_called = false;
  cuke::registry().push_step(cuke::internal::step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&)
      { step1_called = true; }, "first step"));
  cuke::registry().push_step(cuke::internal::step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&)
      { step2_called = true; }, "second step"));

  const char* script = R"*(
    Feature: a feature
    Scenario: First Scenario
    Given first step
    Scenario: Second Scenario
    Given second step
  )*";

  cuke::parser p;
  p.parse_script(script);

  cuke::test_runner runner;
  runner.setup();
  p.for_each_scenario(runner);
  runner.teardown();

  EXPECT_TRUE(step1_called);
  EXPECT_TRUE(step2_called);
  EXPECT_EQ(cuke::results::test_results().scenarios_passed(), 2);
  EXPECT_EQ(cuke::results::final_result(), cuke::results::test_status::failed);

  [[maybe_unused]] auto& clear_args = cuke::internal::get_program_args(0, {});
  clear_args.clear();
}
class hooks_after_all : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    hook_called = false;
    step_called = false;
    cuke::registry().clear();
    cuke::results::test_results().clear();

    cuke::registry().push_hook_after_all(
        cuke::internal::hook([]() { hook_called = true; }));

    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&)
        {
          ASSERT_FALSE(hook_called);
          step_called = true;
        },
        "a step"));
  }

  void TearDown() override
  {
    cuke::registry().clear();
    cuke::results::test_results().clear();
  }

  static bool step_called;
  static bool step_after_called;
  static bool hook_called;
};
bool hooks_after_all::step_called = false;
bool hooks_after_all::step_after_called = false;
bool hooks_after_all::hook_called = false;

TEST_F(hooks_after_all, run_scenario)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given a step 
  )*";

  cuke::parser p;
  p.parse_script(script);

  cuke::test_runner runner;
  runner.setup();
  p.for_each_scenario(runner);
  runner.teardown();

  EXPECT_TRUE(hooks_after_all::hook_called);
  EXPECT_TRUE(hooks_after_all::step_called);
}

TEST_F(hooks_after_all, failing_assert_in_after_all_fails_run)
{
  cuke::registry().clear();
  cuke::results::test_results().clear();

  cuke::registry().push_hook_after_all(
      cuke::internal::hook([]() { cuke::equal(1, 2); }));

  step_after_called = false;
  cuke::registry().push_step(cuke::internal::step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&)
      { step_after_called = true; }, "a step"));

  const char* script = R"*(
    Feature: a feature
    Scenario: First Scenario
    Given a step
  )*";

  cuke::parser p;
  p.parse_script(script);

  cuke::test_runner runner;
  runner.setup();
  p.for_each_scenario(runner);
  runner.teardown();

  EXPECT_TRUE(step_after_called);
  EXPECT_EQ(cuke::results::final_result(), cuke::results::test_status::failed);
}
