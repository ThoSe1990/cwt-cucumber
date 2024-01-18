#include <gtest/gtest.h>

// #include "../src/cwt/vm.hpp"
#include "../src/cwt/cucumber.hpp"

using namespace cwt::details;

TEST(vm, init_obj) { vm test_vm; }
TEST(vm, run_chunk)
{
  vm test_vm;
  test_vm.interpret("Feature:");
}

STEP(some_step, "Any Step")
{
  std::cout << "hello again!!!" << std::endl;
}

TEST(vm, first_feature)
{
  const char* script = R"*(
  Feature: A Fancy Feature
  Scenario: A Scenario
  Given Any Step
)*";
  vm test_vm;

  test_vm.interpret(script);
}