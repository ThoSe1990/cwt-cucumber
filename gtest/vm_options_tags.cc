#include <gtest/gtest.h>

#include "../src/cucumber.hpp"

using namespace cuke::internal;

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
  test_vm.set_options(options{.tags=cuke::internal::tag_expression("@tag1")});
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
  test_vm.set_options(options{.tags=cuke::internal::tag_expression("@tag1")});
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

TEST_F(vm_options_tags, run_0_scenarios_w_feature_tag)
{
const char* script = R"*(
  @tag1
  Feature: First Feature

  Scenario: First Scenario
  Given A Step

  
  Scenario: First Scenario
  Given A Step
)*";
  test_vm.set_options(options{.tags=cuke::internal::tag_expression("@not_given")});

  EXPECT_EQ(return_code::passed, test_vm.run(script));
  EXPECT_EQ(0, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(0, test_vm.step_results().at(return_code::passed));
}

TEST_F(vm_options_tags, run_1_scenarios_w_feature_tag)
{
const char* script = R"*(
  @tag1
  Feature: First Feature

  @tag2
  Scenario: First Scenario
  Given A Step
)*";
  test_vm.set_options(options{.tags=cuke::internal::tag_expression("@tag2")});

  EXPECT_EQ(return_code::passed, test_vm.run(script));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::passed));
}
