#include <gtest/gtest.h>

#include "../src/cwt/cucumber.hpp"

using namespace cwt::details;

class vm_run_scenarios : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    test_vm = vm();
    test_vm.push_step(step([](argc, argv) {}, "A Step with {int}"));
    test_vm.push_step(
        step([](argc, argv) { cuke::is_true(false); }, "This fails"));
  }

  void TearDown() override { test_vm.reset(); }

  vm test_vm;
};

TEST_F(vm_run_scenarios, run_simple_scenario)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step with 123
)*";

  EXPECT_EQ(return_code::passed, test_vm.interpret(script));
  // testing::internal::CaptureStderr();
  // EXPECT_EQ(std::string("[line 1] Error at end: Expect ScenarioLine\n"),
  //           testing::internal::GetCapturedStderr());
  // testing::internal::GetCapturedStdout();
}
TEST_F(vm_run_scenarios, run_simple_scenario_fails)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step with 123
  Given This fails
)*";

  EXPECT_EQ(return_code::failed, test_vm.interpret(script));
}
TEST_F(vm_run_scenarios, run_simple_scenario_outline)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario Outline: First Scenario Outline
  Given A Step with <value>

  Examples:
  | value |
  | 123   |
  | 456   |
)*";

  EXPECT_EQ(return_code::passed, test_vm.interpret(script));
  // testing::internal::CaptureStderr();
  // EXPECT_EQ(std::string("[line 1] Error at end: Expect ScenarioLine\n"),
  //           testing::internal::GetCapturedStderr());
  // testing::internal::CaptureStdout();
  // testing::internal::GetCapturedStdout();
}
