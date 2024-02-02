#include <gtest/gtest.h>

#include "../src/cwt/cucumber.hpp"
#include "../src/cwt/hooks.hpp"

using namespace cwt::details;

class hook_tests : public ::testing::Test
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
    Scenario: A Scenario
    Given A Step
  )*";
  vm test_vm;
};

struct hook_test_exception
{
};

TEST_F(hook_tests, before)
{
  std::size_t call_count = 0;
  test_vm.push_hook_before(hook([]() { throw hook_test_exception{}; }));

  EXPECT_THROW([[maybe_unused]] auto result = test_vm.interpret(script),
               hook_test_exception);
}
TEST_F(hook_tests, after)
{
  std::size_t call_count = 0;
  test_vm.push_hook_after(hook([]() { throw hook_test_exception{}; }));

  EXPECT_THROW([[maybe_unused]] auto result = test_vm.interpret(script),
               hook_test_exception);
}
TEST_F(hook_tests, before_step)
{
  std::size_t call_count = 0;
  test_vm.push_hook_before_step(hook([]() { throw hook_test_exception{}; }));

  EXPECT_THROW([[maybe_unused]] auto result = test_vm.interpret(script),
               hook_test_exception);
}
TEST_F(hook_tests, after_step)
{
  std::size_t call_count = 0;
  test_vm.push_hook_after_step(hook([]() { throw hook_test_exception{}; }));

  EXPECT_THROW([[maybe_unused]] auto result = test_vm.interpret(script),
               hook_test_exception);
}