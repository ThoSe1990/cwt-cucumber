#include <gtest/gtest.h>

#include "../src/cucumber.hpp"

using namespace cwt::details;

class vm_run_scenarios : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    test_vm = vm();
    test_vm.push_step(step([](argc, argv) {}, "A Step"));
    test_vm.push_step(step([](argc, argv) {}, "A Step with {int}"));
  }

  void TearDown() override { test_vm.reset(); }

  vm test_vm;
};

TEST_F(vm_run_scenarios, run_one_scenario)
{
  const char* script = R"*(
  Feature: First Feature
        Following missbehavior during extension development
        with extensions in plan, the stage transition 'tx = {50,60}' was triggered 
        when we were in stage 4 (default stage in first transition)
        This means transitions with extensions were triggered during plan running and forking.
        Its a runtime problem, bc validation was good (as expected)
  Scenario: First Scenario
  Given A Step

)*";
  file f{"no path", script, {}};
  ASSERT_EQ(return_code::passed, test_vm.run(f));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::passed));
}

TEST_F(vm_run_scenarios, run_one_scenario_w_docstring_1)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step
      ```
      Following missbehavior during extension development
      with extensions in plan, the stage transition 'tx = {50,60}' was triggered 
      when we were in stage 4 (default stage in first transition)
      This means transitions with extensions were triggered during plan running and forking.
      Its a runtime problem, bc validation was good (as expected)
      ```
  Scenario: Second Scenario
    Given A Step
)*";
  file f{"no path", script, {}};
  ASSERT_EQ(return_code::passed, test_vm.run(f));
  EXPECT_EQ(2, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(2, test_vm.step_results().at(return_code::passed));
}

TEST_F(vm_run_scenarios, run_one_scenario_w_docstrin_2)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step
      ```
      graph = {
          {1,2,15},
          {2,1,15}
      }
      ```
  Scenario: Second Scenario
    Given A Step
)*";
  file f{"no path", script, {}};
  ASSERT_EQ(return_code::passed, test_vm.run(f));
  EXPECT_EQ(2, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(2, test_vm.step_results().at(return_code::passed));
}
TEST_F(vm_run_scenarios, run_one_scenario_w_docstrin_3)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step
      """
      graph = {
          {1,2,15},
          {2,1,15}
      }
      """
    And A Step
  Scenario: Second Scenario
    Given A Step
)*";
  file f{"no path", script, {}};
  ASSERT_EQ(return_code::passed, test_vm.run(f));
  EXPECT_EQ(2, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(3, test_vm.step_results().at(return_code::passed));
}
