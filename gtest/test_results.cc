#include <gtest/gtest.h>

#include "../src/ast/parser.hpp"
#include "../src/ast/test_runner.hpp"
#include "../src/ast/test_results.hpp"

class test_results_1 : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    call_count = 0;
    cuke::results::test_results().clear(); 
    cuke::registry().clear();
    cuke::registry().push_step(cuke::internal::step(
        [](const cuke::value_array&) { test_results_1::call_count++; },
        "a step"));
  }

  static std::size_t call_count;
};
std::size_t test_results_1::call_count = 0;

TEST_F(test_results_1, run_scenario)
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

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
}
