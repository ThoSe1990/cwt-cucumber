#include <gtest/gtest.h>

#include "../src/cwt/vm.hpp"

using namespace cwt::details;

TEST(vm, init_obj) { vm test_vm; }
TEST(vm, run_chunk)
{
  vm test_vm;
  test_vm.interpret("Feature:");
}
TEST(vm, first_feature)
{
  const char* script = R"*(
  Feature: A Fancy Feature
  Scenario: A Scenario
  Given Any Step
)*";
  vm test_vm;
  test_vm.push_step(step([](const value_array&) { std::cout << "this gets called!" << std::endl; },
                    "Any Step"));
  test_vm.interpret(script);
}