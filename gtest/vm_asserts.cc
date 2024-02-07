#include <gtest/gtest.h>

#include "../src/cwt/cucumber.hpp"

using namespace cwt::details;

class vm_asserts : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    test_vm = vm();
    test_vm.push_step(step([](argc, argv) {}, "A Step with {int}"));
  }

  void TearDown() override { test_vm.reset(); }

  vm test_vm;
};

TEST_F(vm_asserts, run_simple_scenario)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step with 123
)*";

  EXPECT_EQ(return_code::passed, test_vm.interpret(script));

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
