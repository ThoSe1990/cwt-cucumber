#include <gtest/gtest.h>

#include "../src/cwt/cucumber.hpp"

using namespace cwt::details;

class vm_options_quiet : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    test_vm = vm();
    test_vm.push_step(step([](argc, argv) {  }, "A Step"));
  }

  void TearDown() override { test_vm.reset(); }

  vm test_vm;

};

TEST_F(vm_options_quiet, run_scenario)
{
const char* script = R"*(
  Feature: First Feature

  Scenario: First Scenario
  Given A Step
)*";
  test_vm.set_options(options{.quiet=true});
  EXPECT_EQ(return_code::passed, test_vm.interpret(script));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::passed));
}
