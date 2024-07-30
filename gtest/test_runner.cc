#include <gtest/gtest.h>

#include "../src/ast/test_runner.hpp"
#include "../src/ast/parser.hpp"

class test_runner : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    call_count = 0;
    cuke::registry::clear();
    cuke::registry::push_step(cuke::internal::step(
        [](const cuke::value_array&) { test_runner::call_count++; }, "a step"));
  }

  static std::size_t call_count;
};
std::size_t test_runner::call_count = 0;

TEST_F(test_runner, run_step)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given a step 
  )*";

  cuke::parser p;
  p.parse_script(script);
  cuke::internal::test_runner visitor;
  p.for_each_scenario(visitor);
  EXPECT_EQ(test_runner::call_count, 1);
}
