#include <gtest/gtest.h>

#include "../src/cwt/cucumber.hpp"

using namespace cwt::details;

class vm_run_scenarios : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    test_vm = vm();
    test_vm.push_step(step([](argc, argv) {}, "A Step"));
  }

  void TearDown() override { test_vm.reset(); }

  vm test_vm;
};


TEST_F(vm_run_scenarios, first_run)
{
const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step
)*";

  EXPECT_EQ(return_code::passed, test_vm.interpret(script));
  // testing::internal::CaptureStderr();
  // EXPECT_EQ(std::string("[line 1] Error at end: Expect ScenarioLine\n"),
  //           testing::internal::GetCapturedStderr());
  // testing::internal::CaptureStdout();
  // testing::internal::GetCapturedStdout();
}
