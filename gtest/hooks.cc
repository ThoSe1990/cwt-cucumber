#include <gtest/gtest.h>

#include "../src/cucumber.hpp"
#include "../src/hooks.hpp"

using namespace cwt::details;

class hooks_tests : public ::testing::Test
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

TEST_F(hooks_tests, before)
{
  test_vm.push_hook_before(hook([]() { throw hook_test_exception{}; }));
  EXPECT_THROW([[maybe_unused]] auto result = test_vm.run(script),
               hook_test_exception);
}
TEST_F(hooks_tests, after)
{
  test_vm.push_hook_after(hook([]() { throw hook_test_exception{}; }));
  EXPECT_THROW([[maybe_unused]] auto result = test_vm.run(script),
               hook_test_exception);
}
TEST_F(hooks_tests, before_step)
{
  test_vm.push_hook_before_step(hook([]() { throw hook_test_exception{}; }));
  EXPECT_THROW([[maybe_unused]] auto result = test_vm.run(script),
               hook_test_exception);
}
TEST_F(hooks_tests, after_step)
{
  test_vm.push_hook_after_step(hook([]() { throw hook_test_exception{}; }));
  EXPECT_THROW([[maybe_unused]] auto result = test_vm.run(script),
               hook_test_exception);
}

TEST_F(hooks_tests, before_tag_no_call)
{
  struct dont_throw_me
  {
  };

  test_vm.push_hook_before(hook([]() { throw hook_test_exception{}; }));
  test_vm.push_hook_after(
      hook([]() { throw dont_throw_me{}; }, "@tag1 or @tag2"));
  EXPECT_THROW([[maybe_unused]] auto result = test_vm.run(script),
               hook_test_exception);
}