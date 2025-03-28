#include <gtest/gtest.h>

#include "../src/test_runner.hpp"
#include "../src/parser.hpp"

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
  static bool step_called;
  static bool hook_called;
};
bool hooks_before_all::step_called = false;
bool hooks_before_all::hook_called = false;

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
class hooks_after_all : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    hook_called = false;
    step_called = false;
    cuke::registry().clear();

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
  static bool step_called;
  static bool hook_called;
};
bool hooks_after_all::step_called = false;
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
