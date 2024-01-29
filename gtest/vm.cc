#include <gtest/gtest.h>

// #include "../src/cwt/vm.hpp"
#include "../src/cwt/cucumber.hpp"

using namespace cwt::details;

TEST(vm, init_obj) { vm test_vm; }
TEST(vm, run_chunk)
{
  vm test_vm;
  [[maybe_unused]] return_code result = test_vm.interpret("Feature:");
}

STEP(some_step, "Any Step")
{
  std::cout << "**************** Any Step" << std::endl;
}
STEP(some_step2, "Any Step with {string}")
{
  std::cout << "**************** Any Step " << values->as<std::string>() << std::endl;
}
STEP(some_step_with_int, "A Step with {int}")
{
  std::cout << "**************** n = " << n << "  -> " << values->as<int>() << std::endl;
}

STEP(another_step, "Another Step")
{
  std::cout << "**************** Another Step" << std::endl;
}

BEFORE(some_hook)
{
  std::cout << "BEFORE(some_hook)" << std::endl;
}
BEFORE(some_hook2)
{
  std::cout << "BEFORE(some_hook2)" << std::endl;
}
AFTER(some_hook3)
{
  std::cout << "AFTER(some_hook3)" << std::endl;
}
AFTER(some_hook4)
{
  std::cout << "AFTER(some_hook4)" << std::endl;
}

BEFORE_STEP(some_hook5)
{
  std::cout << "BEFORE_STEP(some_hook5)" << std::endl;
}
BEFORE_STEP(some_hook6)
{
  std::cout << "BEFORE_STEP(some_hook6)" << std::endl;
}
AFTER_STEP(some_hook7)
{
  std::cout << "AFTER_STEP(some_hook7)" << std::endl;
}
AFTER_STEP(some_hook8)
{
  std::cout << "AFTER_STEP(some_hook8)" << std::endl;
}

TEST(vm, first_feature)
{
  const char* script = R"*(
  Feature: A Fancy Feature
  Scenario: A Scenario
  Given Any Step
  Given Any Step with "hello world!!!"
  Given Another Step
  Given A Step with 11
  Given A Step with 22
)*";
  vm test_vm;

  [[maybe_unused]] return_code result = test_vm.interpret(script);
}

