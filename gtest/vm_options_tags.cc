#include <gtest/gtest.h>

#include "../src/cwt/cucumber.hpp"

using namespace cwt::details;

class vm_options_tags : public ::testing::Test
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

TEST_F(vm_options_tags, ignore_scenario)
{
const char* script = R"*(
  Feature: First Feature

  Scenario: First Scenario
  Given A Step

  @any_tag
  Scenario: First Scenario
  Given A Step
)*";
  test_vm.set_options(options{.tags=compiler::tag_expression("@tag1")});
  EXPECT_EQ(return_code::passed, test_vm.run(script));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::passed));
}
TEST_F(vm_options_tags, run_scenario_w_tag)
{
const char* script = R"*(
  Feature: First Feature

  Scenario: First Scenario
  Given A Step

  @tag1
  Scenario: First Scenario
  Given A Step
)*";
  test_vm.set_options(options{.tags=compiler::tag_expression("@tag1")});
  EXPECT_EQ(return_code::passed, test_vm.run(script));
  EXPECT_EQ(2, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(2, test_vm.step_results().at(return_code::passed));
}
TEST_F(vm_options_tags, no_tags)
{
const char* script = R"*(
  Feature: First Feature

  Scenario: First Scenario
  Given A Step

  @tag1
  Scenario: First Scenario
  Given A Step
)*";

  EXPECT_EQ(return_code::passed, test_vm.run(script));
  EXPECT_EQ(2, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(2, test_vm.step_results().at(return_code::passed));
}
