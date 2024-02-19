#include <gtest/gtest.h>

#include "../src/cucumber.hpp"

using namespace cwt::details;

class vm_asserts_is_true : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    test_vm = vm();
    test_vm.push_step(step([](argc, argv) { cuke::is_true(true); }, "Is true"));
    test_vm.push_step(step([](argc, argv) { cuke::is_true(false); }, "Is not true"));
  }

  void TearDown() override { test_vm.reset(); }

  vm test_vm;

};

TEST_F(vm_asserts_is_true, pass)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given Is true
)*";

  EXPECT_EQ(return_code::passed, test_vm.run(script));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::passed));
}
