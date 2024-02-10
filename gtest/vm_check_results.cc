#include <gtest/gtest.h>

#include "../src/cwt/cucumber.hpp"

using namespace cwt::details;

class vm_check_results : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    test_vm = vm();
    test_vm.push_step(step([](argc, argv) {}, "A Step with {int}"));
    test_vm.push_step(step(
        [](argc, argv) { throw std::runtime_error("This shall not happen!"); },
        "This throws"));
    test_vm.push_step(
        step([](argc, argv) { cuke::is_true(false); }, "This fails"));
  }

  void TearDown() override { test_vm.reset(); }

  vm test_vm;
};

TEST_F(vm_check_results, run_simple_scenario)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step with 123
)*";

  EXPECT_EQ(return_code::passed, test_vm.run(script));

  EXPECT_EQ(4, test_vm.scenario_results().size());
  EXPECT_EQ(0, test_vm.scenario_results().at(return_code::failed));
  EXPECT_EQ(0, test_vm.scenario_results().at(return_code::skipped));
  EXPECT_EQ(0, test_vm.scenario_results().at(return_code::undefined));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::passed));

  EXPECT_EQ(4, test_vm.step_results().size());
  EXPECT_EQ(0, test_vm.step_results().at(return_code::failed));
  EXPECT_EQ(0, test_vm.step_results().at(return_code::skipped));
  EXPECT_EQ(0, test_vm.step_results().at(return_code::undefined));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::passed));
}
TEST_F(vm_check_results, run_simple_scenario_fails)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step with 123
  Given This fails
)*";

  EXPECT_EQ(return_code::failed, test_vm.run(script));

  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::failed));
  EXPECT_EQ(0, test_vm.scenario_results().at(return_code::skipped));
  EXPECT_EQ(0, test_vm.scenario_results().at(return_code::undefined));
  EXPECT_EQ(0, test_vm.scenario_results().at(return_code::passed));

  EXPECT_EQ(1, test_vm.step_results().at(return_code::failed));
  EXPECT_EQ(0, test_vm.step_results().at(return_code::skipped));
  EXPECT_EQ(0, test_vm.step_results().at(return_code::undefined));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::passed));
}
TEST_F(vm_check_results, simple_scenario_with_undefined_steps)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step with 123
  * This is not implemented
  * This throws
)*";

  EXPECT_EQ(return_code::failed, test_vm.run(script));

  EXPECT_EQ(0, test_vm.scenario_results().at(return_code::failed));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::skipped));
  EXPECT_EQ(0, test_vm.scenario_results().at(return_code::undefined));
  EXPECT_EQ(0, test_vm.scenario_results().at(return_code::passed));

  EXPECT_EQ(0, test_vm.step_results().at(return_code::failed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::skipped));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::undefined));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::passed));
}
TEST_F(vm_check_results, run_simple_scenario_fails_steps_skipped_1)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step with 123
  Given This fails
  Given This throws
  Given This throws
)*";

  EXPECT_EQ(return_code::failed, test_vm.run(script));

  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::failed));
  EXPECT_EQ(0, test_vm.scenario_results().at(return_code::skipped));
  EXPECT_EQ(0, test_vm.scenario_results().at(return_code::undefined));
  EXPECT_EQ(0, test_vm.scenario_results().at(return_code::passed));

  EXPECT_EQ(1, test_vm.step_results().at(return_code::failed));
  EXPECT_EQ(2, test_vm.step_results().at(return_code::skipped));
  EXPECT_EQ(0, test_vm.step_results().at(return_code::undefined));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::passed));
}
TEST_F(vm_check_results, run_simple_scenario_fails_steps_skipped_2)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step with 123
  Given This fails
  Given This throws
  Given This throws
  # and another one ... 
  Scenario: Second Scenario
  Given A Step with 123
  Given This fails
  Given This throws
  Given This throws
)*";

  EXPECT_EQ(return_code::failed, test_vm.run(script));

  EXPECT_EQ(2, test_vm.scenario_results().at(return_code::failed));
  EXPECT_EQ(0, test_vm.scenario_results().at(return_code::skipped));
  EXPECT_EQ(0, test_vm.scenario_results().at(return_code::undefined));
  EXPECT_EQ(0, test_vm.scenario_results().at(return_code::passed));

  EXPECT_EQ(2, test_vm.step_results().at(return_code::failed));
  EXPECT_EQ(4, test_vm.step_results().at(return_code::skipped));
  EXPECT_EQ(0, test_vm.step_results().at(return_code::undefined));
  EXPECT_EQ(2, test_vm.step_results().at(return_code::passed));
}
TEST_F(vm_check_results, run_simple_scenario_outline)
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

  EXPECT_EQ(return_code::passed, test_vm.run(script));

  EXPECT_EQ(0, test_vm.scenario_results().at(return_code::failed));
  EXPECT_EQ(0, test_vm.scenario_results().at(return_code::skipped));
  EXPECT_EQ(0, test_vm.scenario_results().at(return_code::undefined));
  EXPECT_EQ(2, test_vm.scenario_results().at(return_code::passed));

  EXPECT_EQ(0, test_vm.step_results().at(return_code::failed));
  EXPECT_EQ(0, test_vm.step_results().at(return_code::skipped));
  EXPECT_EQ(0, test_vm.step_results().at(return_code::undefined));
  EXPECT_EQ(2, test_vm.step_results().at(return_code::passed));
}


