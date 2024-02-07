#include <gtest/gtest.h>

#include "../src/cwt/cucumber.hpp"

using namespace cwt::details;

class vm_get_args : public ::testing::Test
{
 public:
  static std::size_t integral;

 protected:
  void SetUp() override
  {
    integral = 0;
    test_vm = vm();
    test_vm.push_step(
        step([](argc n, argv values) {
          int i0 = cwt::details::get_arg(n, values, 1, __FILE__, __LINE__);
          int i1 = cwt::details::get_arg(n, values, 2, __FILE__, __LINE__);
          
          std::cout << i0 << ' ' << i1 << std::endl;
          
          }, "A Step with {int} and {int}"));
  }

  void TearDown() override { test_vm.reset(); }

  vm test_vm;
};
std::size_t vm_get_args::integral;

TEST_F(vm_get_args, run_simple_scenario)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  * A Step with 1 and 2"
  * A Step with 3 and 4
)*";
  [[maybe_unused]] return_code r = test_vm.interpret(script);
}
