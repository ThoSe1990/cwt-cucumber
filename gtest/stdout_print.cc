
#include <gtest/gtest.h>

#include "../src/ast/test_runner.hpp"
#include "../src/ast/parser.hpp"

class stdout_print : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    cuke::registry().clear();
    cuke::registry().push_step(
        cuke::internal::step([](const cuke::value_array&) {}, "a step"));
  }

  static std::size_t call_count;
};

TEST_F(stdout_print, scenario_pass)
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
}
