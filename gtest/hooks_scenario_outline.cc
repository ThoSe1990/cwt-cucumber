#include <gtest/gtest.h>

#include "../src/cucumber.hpp"
#include "../src/hooks.hpp"

using namespace cwt::details;

namespace hooks_scenario_outline_call_count
{
std::size_t value = 0;
}

class hooks_scenario_outline : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    hooks_scenario_outline_call_count::value = 0;
    test_vm = vm();
    test_vm.push_step(step([](argc, argv) {}, "A Step with a {string}"));
  }

  void TearDown() override { test_vm.reset(); }
  const char* script = R"*(
    Feature: A Feature

    Scenario Outline: A Scenario Outline
    Given A Step with a <value>

    Examples:
    | value   | 
    | "hello" |

    @tag1
    Examples:
    | value   | 
    | "world" |

    @tag2 
    Scenario Outline: A Scenario Outline
    Given A Step with a <value>
    
    @tag3 @tag4
    Examples:
    | value     | 
    | "another" |

    Examples:
    | value   | 
    | "test"  |

  )*";
  vm test_vm;
};

TEST_F(hooks_scenario_outline, before_call_hook)
{
  test_vm.push_hook_before(
      hook([]() { ++hooks_scenario_outline_call_count::value; }));
  EXPECT_EQ(test_vm.run(script), return_code::passed);
  EXPECT_EQ(hooks_scenario_outline_call_count::value, 4);
}
TEST_F(hooks_scenario_outline, after_call_hook)
{
  test_vm.push_hook_before(
      hook([]() { ++hooks_scenario_outline_call_count::value; }));
  EXPECT_EQ(test_vm.run(script), return_code::passed);
  EXPECT_EQ(hooks_scenario_outline_call_count::value, 4);
}
