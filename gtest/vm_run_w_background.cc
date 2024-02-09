#include <gtest/gtest.h>

#include "../src/cwt/cucumber.hpp"

using namespace cwt::details;

class vm_run_w_background : public ::testing::Test
{
 public:
  static std::size_t call_count;

 protected:
  void SetUp() override
  {
    call_count = 0;
    test_vm = vm();
    test_vm.push_step(step([](argc, argv) { ++call_count; }, "Call me"));
    test_vm.push_step(step([](argc, argv) { ++call_count; }, "Call me with {int}"));
    test_vm.push_step(step([](argc, argv) { cuke::is_true(false); }, "This fails"));
  }

  void TearDown() override { test_vm.reset(); }

  vm test_vm;
};
std::size_t vm_run_w_background::call_count;

TEST_F(vm_run_w_background, scenario_calls)
{
  const char* script = R"*(
  Feature: First Feature

  Background:
  * Call me
  * Call me
  
  Scenario: First Scenario
  * Call me
)*";

  EXPECT_EQ(test_vm.interpret(script), return_code::passed);
  EXPECT_EQ(vm_run_w_background::call_count, 3);
  EXPECT_EQ(test_vm.scenario_results().at(return_code::passed), 1);
  EXPECT_EQ(test_vm.step_results().at(return_code::passed), 3);
}
TEST_F(vm_run_w_background, background_fails)
{
  const char* script = R"*(
  Feature: First Feature

  Background:
  * This fails
  
  Scenario: First Scenario
  * Call me
  * Call me
)*";

  EXPECT_EQ(test_vm.interpret(script), return_code::failed);
  EXPECT_EQ(vm_run_w_background::call_count, 0);
  EXPECT_EQ(test_vm.scenario_results().at(return_code::failed), 1);
  EXPECT_EQ(test_vm.step_results().at(return_code::failed), 1);
  EXPECT_EQ(test_vm.step_results().at(return_code::skipped), 2);
}
TEST_F(vm_run_w_background, background_undefined_step)
{
  const char* script = R"*(
  Feature: First Feature

  Background:
  * Undefined Step
  
  Scenario: First Scenario
  * Call me
  * Call me
)*";

  EXPECT_EQ(test_vm.interpret(script), return_code::failed);
  EXPECT_EQ(vm_run_w_background::call_count, 0);
  EXPECT_EQ(test_vm.scenario_results().at(return_code::skipped), 1);
  EXPECT_EQ(test_vm.step_results().at(return_code::undefined), 1);
  EXPECT_EQ(test_vm.step_results().at(return_code::skipped), 2);
}
TEST_F(vm_run_w_background, scenario_calls_with_description)
{
  const char* script = R"*(
  Feature: First Feature

  Background:
  some description here ... 
  some description here ... 
  some description here ... 
  * Call me
  * Call me
  
  Scenario: First Scenario
  * Call me
)*";

  EXPECT_EQ(test_vm.interpret(script), return_code::passed);
  EXPECT_EQ(vm_run_w_background::call_count, 3);
  EXPECT_EQ(test_vm.scenario_results().at(return_code::passed), 1);
  EXPECT_EQ(test_vm.step_results().at(return_code::passed), 3);
}
TEST_F(vm_run_w_background, scenario_outline_calls)
{
  const char* script = R"*(
  Feature: First Feature

  Background:
  * Call me
  * Call me
  
  Scenario Outline: First Scenario
  * Call me with <value>

  Examples:
  | value |
  | 123   |
  | 123   |
)*";

  EXPECT_EQ(test_vm.interpret(script), return_code::passed);
  EXPECT_EQ(vm_run_w_background::call_count, 6);
  EXPECT_EQ(test_vm.scenario_results().at(return_code::passed), 2);
  EXPECT_EQ(test_vm.step_results().at(return_code::passed), 6);
}