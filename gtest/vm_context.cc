#include <gtest/gtest.h>

#include "../src/cwt/cucumber.hpp"

using namespace cwt::details;

class vm_context_init_values : public ::testing::Test
{
  struct foo
  {
  };
  struct bar
  {
  };

 protected:
  void SetUp() override
  {
    call_count = 0;
    test_vm = vm();
    test_vm.push_step(step(
        [](argc, argv)
        {
          ++call_count;
          EXPECT_EQ(cwt::details::get_context().size(), 0);
        },
        "Initial step"));
    test_vm.push_step(step(
        [](argc, argv)
        {
          ++call_count;
          cuke::context<foo>();
          cuke::context<bar>();
          cuke::context<std::string>("hello world");
          cuke::context<int>(999);
          EXPECT_EQ(cwt::details::get_context().size(), 4);
        },
        "Here we set"));
    test_vm.push_step(step(
        [](argc, argv)
        {
          ++call_count;
          EXPECT_EQ(cuke::context<std::string>(), std::string("hello world"));
          EXPECT_EQ(cuke::context<int>(), 999);
          EXPECT_EQ(cwt::details::get_context().size(), 4);
        },
        "Here we check"));
  }

  void TearDown() override { test_vm.reset(); }

  vm test_vm;
  static std::size_t call_count;
};
std::size_t vm_context_init_values::call_count;

TEST_F(vm_context_init_values, initialize_values)
{
  const char* script = R"*(
  Feature: First Feature

  Scenario: First Scenario
  Given Initial step
  When Here we set
  Then Here we check
)*";
  EXPECT_EQ(return_code::passed, test_vm.run(script));
  EXPECT_EQ(call_count, 3);
}
TEST_F(vm_context_init_values, reset_in_next_scenario)
{
  const char* script = R"*(
  Feature: First Feature

  Scenario: First Scenario
  Given Initial step
  When Here we set
  Then Here we check

  Scenario: First Scenario
  Given Initial step
)*";
  EXPECT_EQ(return_code::passed, test_vm.run(script));
  EXPECT_EQ(call_count, 4);
}

class vm_context_modify_values : public ::testing::Test
{
  struct foo
  {
    int value;
  };

 protected:
  void SetUp() override
  {
    call_count = 0;
    test_vm = vm();
    test_vm.push_step(step(
        [](argc n, argv values)
        {
          ++call_count;
          EXPECT_EQ(cwt::details::get_context().size(), 0);
          int value = CUKE_ARG(1);
          cuke::context<foo>(foo{value});
          EXPECT_EQ(cwt::details::get_context().size(), 1);
        },
        "Init foo with {int}"));
    test_vm.push_step(step(
        [](argc n, argv values)
        {
          ++call_count;
          int old_value = CUKE_ARG(1);
          int new_value = CUKE_ARG(2);
          EXPECT_EQ(old_value, cuke::context<foo>().value);
          cuke::equal(old_value, cuke::context<foo>().value);
          cuke::context<foo>().value = new_value;
        },
        "Then foo was {int} but now foo is {int}"));
    test_vm.push_step(step(
        [](argc n, argv values)
        {
          ++call_count;
          int value = CUKE_ARG(1);
          EXPECT_EQ(value, cuke::context<foo>().value);
          cuke::equal(value, cuke::context<foo>().value);
        },
        "foo finally is {int}"));
    test_vm.push_step(step(
        [](argc n, argv values)
        {
          ++call_count;
          int value = CUKE_ARG(1);
          EXPECT_FALSE(value == cuke::context<foo>().value);
          cuke::equal(value, cuke::context<foo>().value);
        },
        "foo is not {int}"));
  }

  void TearDown() override { test_vm.reset(); }

  vm test_vm;
  static std::size_t call_count;
};
std::size_t vm_context_modify_values::call_count;

TEST_F(vm_context_modify_values, set_foo_pass)
{
  const char* script = R"*(
  Feature: First Feature

  Scenario: First Scenario
  Given Init foo with 12
  When Then foo was 12 but now foo is 24
  Then foo finally is 24
)*";
  EXPECT_EQ(return_code::passed, test_vm.run(script));
  EXPECT_EQ(call_count, 3);
}

TEST_F(vm_context_modify_values, set_foo_fail)
{
  const char* script = R"*(
  Feature: First Feature

  Scenario: First Scenario
  Given Init foo with 12
  Then foo is not 24
)*";
  EXPECT_EQ(return_code::failed, test_vm.run(script));
  EXPECT_EQ(call_count, 2);
}


TEST_F(vm_context_modify_values, scenario_outline)
{
  const char* script = R"*(
  Feature: First Feature

  Scenario Outline: First Scenario
  Given Init foo with <init value>
  When Then foo was <init value> but now foo is <new value>
  Then foo finally is <new value>

  Examples:
  | init value | new value |
  | 1          | 1         |
  | 2          | 2         |
  | 3          | 3         |
  | 4          | 4         |
)*";
  EXPECT_EQ(return_code::passed, test_vm.run(script));
  EXPECT_EQ(call_count, 12);
}

TEST_F(vm_context_modify_values, scenario_and_scenario_outline)
{
  const char* script = R"*(
  Feature: First Feature

  Scenario: First Scenario
  Given Init foo with 12
  Then foo finally is 12

  Scenario Outline: First Scenario
  Given Init foo with <init value>
  When Then foo was <init value> but now foo is <new value>
  Then foo finally is <new value>

  Examples:
  | init value | new value |
  | 11         | 88        |
  | 123        | 345       |
)*";
  EXPECT_EQ(return_code::passed, test_vm.run(script));
  EXPECT_EQ(call_count, 8);
}