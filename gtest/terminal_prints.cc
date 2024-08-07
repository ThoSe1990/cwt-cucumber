
#include <gtest/gtest.h>

#include "../src/ast/parser.hpp"
#include "../src/ast/test_runner.hpp"
#include "../src/ast/test_results.hpp"
#include "../src/asserts.hpp"

class terminal_prints : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    cuke::results::test_results().clear();
    cuke::registry().clear();
    cuke::registry().push_step(
        cuke::internal::step([](const cuke::value_array&) {}, "a step"));
    cuke::registry().push_step(cuke::internal::step(
        [](const cuke::value_array&) { cuke::is_true(false); }, "this fails"));
  }
};

TEST_F(terminal_prints, scenario_passed)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given a step 
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);
  // TODO: implement stdout capture and compare
}
