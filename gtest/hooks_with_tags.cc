#include <gtest/gtest.h>

#include "../src/cwt/cucumber.hpp"
#include "../src/cwt/hooks.hpp"

using namespace cwt::details;

class hooks_tests_with_tags : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    test_vm = vm();
    test_vm.push_step(step([](argc, argv) {}, "A Step"));
  }

  void TearDown() override { test_vm.reset(); }
  const char* script = R"*(
    Feature: A Feature

    @tag1 
    Scenario: A Scenario
    Given A Step

    @tag2 @tag3
    Scenario: A Scenario
    Given A Step
  )*";
  vm test_vm;
};

struct hook_test_exception
{
};

TEST_F(hooks_tests_with_tags, call_hook)
{
  test_vm.push_hook_after(
      hook([]() { throw hook_test_exception{}; }, "@tag1"));
  EXPECT_THROW([[maybe_unused]] auto result = test_vm.interpret(script),
               hook_test_exception);
}
TEST_F(hooks_tests_with_tags, dont_call_hook)
{
  test_vm.push_hook_after(
      hook([]() { throw hook_test_exception{}; }, "@some_other_tag"));
  EXPECT_NO_THROW([[maybe_unused]] auto result = test_vm.interpret(script));
}