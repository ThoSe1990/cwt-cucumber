#include <gtest/gtest.h>

#include "../src/cucumber.hpp"
#include "../src/hooks.hpp"

using namespace cwt::details;

namespace hooks_tests_call_count
{
std::size_t value = 0;
}

class hooks_tests_with_tags : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    hooks_tests_call_count::value = 0;
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


TEST_F(hooks_tests_with_tags, before_call_hook)
{
  test_vm.push_hook_before(
      hook([]() { ++hooks_tests_call_count::value; }, "@tag1"));
  EXPECT_EQ(test_vm.run(script), return_code::passed);
  EXPECT_EQ(hooks_tests_call_count::value, 1);
}
TEST_F(hooks_tests_with_tags, before_dont_call_hook)
{
  test_vm.push_hook_before(
      hook([]() { ++hooks_tests_call_count::value; }, "@some_other_tag"));
  EXPECT_EQ(test_vm.run(script), return_code::passed);
  EXPECT_EQ(hooks_tests_call_count::value, 0);
}
TEST_F(hooks_tests_with_tags, after_call_hook)
{
  test_vm.push_hook_after(
      hook([]() { ++hooks_tests_call_count::value; }, "@tag1"));
  EXPECT_EQ(test_vm.run(script), return_code::passed);
  EXPECT_EQ(hooks_tests_call_count::value, 1);
}
TEST_F(hooks_tests_with_tags, after_dont_call_hook)
{
  test_vm.push_hook_after(
      hook([]() { ++hooks_tests_call_count::value; }, "@some_other_tag"));
  EXPECT_EQ(test_vm.run(script), return_code::passed);
  EXPECT_EQ(hooks_tests_call_count::value, 0);
}

TEST_F(hooks_tests_with_tags, after_count_calls_1)
{
  test_vm.push_hook_after(
      hook([]() { ++hooks_tests_call_count::value; }, "@tag1 or @tag2"));
  EXPECT_EQ(test_vm.run(script), return_code::passed);
  EXPECT_EQ(hooks_tests_call_count::value, 2);
}
TEST_F(hooks_tests_with_tags, after_count_calls_2)
{
  test_vm.push_hook_after(
      hook([]() { ++hooks_tests_call_count::value; }, "@tag3 and @tag2"));
  EXPECT_EQ(test_vm.run(script), return_code::passed);
  EXPECT_EQ(hooks_tests_call_count::value, 1);
}
TEST_F(hooks_tests_with_tags, after_count_calls_3)
{
  test_vm.push_hook_after(
      hook([]() { ++hooks_tests_call_count::value; }, "@tag3 or @tag2"));
  EXPECT_EQ(test_vm.run(script), return_code::passed);
  EXPECT_EQ(hooks_tests_call_count::value, 1);
}

TEST_F(hooks_tests_with_tags, before_count_calls_1)
{
  test_vm.push_hook_before(
      hook([]() { ++hooks_tests_call_count::value; }, "@tag1 or @tag2"));
  EXPECT_EQ(test_vm.run(script), return_code::passed);
  EXPECT_EQ(hooks_tests_call_count::value, 2);
}
TEST_F(hooks_tests_with_tags, before_count_calls_2)
{
  test_vm.push_hook_before(
      hook([]() { ++hooks_tests_call_count::value; }, "@tag3 and @tag2"));
  EXPECT_EQ(test_vm.run(script), return_code::passed);
  EXPECT_EQ(hooks_tests_call_count::value, 1);
}
