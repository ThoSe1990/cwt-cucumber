#include <gtest/gtest.h>

#include "../src/cwt/cucumber.hpp"
#include "../src/cwt/hooks.hpp"

using namespace cwt::details;

class scenario_outline_tests : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    test_vm = vm();
    test_vm.push_step(step(
        [](argc n, argv values)
        {
          EXPECT_EQ(n, 1);
          EXPECT_EQ(values->as<std::string>(), std::string("hello"));
        },
        "A Step with a {string}"));
    test_vm.push_step(step(
        [](argc n, argv values)
        {
          EXPECT_EQ(n, 1);
          EXPECT_EQ(values->as<int>(), 123);
        },
        "And another with {int}"));
  }

  void TearDown() override { test_vm.reset(); }
  const char* script = R"*(
    Feature: A Feature

    Scenario Outline: A Scenario Outline
    Given A Step with a <var1>
    Given And another with <var2>

    Examples:
    | var1    |  var2 |
    | "hello" |  123  |
  )*";
  vm test_vm;
};

TEST_F(scenario_outline_tests, check_args)
{
  EXPECT_EQ(test_vm.run(script), return_code::passed);
}