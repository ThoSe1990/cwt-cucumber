#include <gtest/gtest.h>

#include "../src/cucumber.hpp"

using namespace cwt::details;

class vm_options_line : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    test_vm = vm();
    test_vm.push_step(step([](argc, argv) {  }, "A Step"));
    test_vm.push_step(step([](argc, argv) {  }, "A Step with {int}"));
  }

  void TearDown() override { test_vm.reset(); }

  vm test_vm;
};

TEST_F(vm_options_line, run_one_scenario)
{
const char* script = R"*(
  Feature: First Feature

  Scenario: First Scenario
  Given A Step

  Scenario: First Scenario
  Given A Step

  Scenario: First Scenario
  Given A Step
)*";
  file f{"no path", script, {7}};
  EXPECT_EQ(return_code::passed, test_vm.run(f));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::passed));
}

TEST_F(vm_options_line, run_multiple_scenarios)
{
const char* script = R"*(
  Feature: First Feature

  Scenario: First Scenario
  Given A Step
  Scenario: First Scenario
  Given A Step
  Scenario: First Scenario
  Given A Step
  Scenario: First Scenario
  Given A Step
  Scenario: First Scenario
  Given A Step
  Scenario: First Scenario
  Given A Step
)*";
  file f{"no path", script, {6,10,12}};
  EXPECT_EQ(return_code::passed, test_vm.run(f));
  EXPECT_EQ(3, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(3, test_vm.step_results().at(return_code::passed));
}
TEST_F(vm_options_line, run_scenarios_outline)
{
const char* script = R"*(
  Feature: First Feature

  Scenario Outline: First Scenario
  Given A Step with <value>

  Examples: 
  | value |
  | 1     |
  | 2     |
  | 3     |
  | 4     |
)*";
  file f{"no path", script, {11,10}};
  EXPECT_EQ(return_code::passed, test_vm.run(f));
  EXPECT_EQ(2, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(2, test_vm.step_results().at(return_code::passed));
}

TEST_F(vm_options_line, run_scenario_and_scenarios_outline)
{
const char* script = R"*(
  Feature: First Feature

  Scenario: First Scenario
  Given A Step
  Scenario: First Scenario
  Given A Step

  Scenario Outline: First Scenario
  Given A Step with <value>

  Examples: 
  | value |
  | 1     |
  | 2     |
  | 3     |
  | 4     |

  Scenario: First Scenario
  Given A Step
  Scenario: First Scenario
  Given A Step
)*";
  file f{"no path", script, {14,6,21,17}};
  EXPECT_EQ(return_code::passed, test_vm.run(f));
  EXPECT_EQ(4, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(4, test_vm.step_results().at(return_code::passed));
}