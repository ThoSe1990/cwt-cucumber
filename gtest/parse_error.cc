#include <gtest/gtest.h>

#include "../src/test_runner.hpp"
#include "../src/parser.hpp"
#include "../src/test_results.hpp"

// parse_impl clears the document when the script does not parse, so anything
// that walks the document afterwards must ask before dereferencing it.
class parse_error : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    cuke::registry().clear();
    cuke::results::test_results().clear();
  }
};

TEST_F(parse_error, for_each_scenario_on_a_rejected_script)
{
  cuke::parser p;
  p.parse_script("not a feature file at all");
  ASSERT_TRUE(p.error());

  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(cuke::results::test_results().scenarios_count(), 0);
}
TEST_F(parse_error, for_each_scenario_on_a_broken_data_table)
{
  const char* script = R"*(
    Feature: a feature
    Scenario: a scenario
    Given a step with a table
      | one | two |
      | only one |
  )*";
  cuke::parser p;
  p.parse_script(script);
  ASSERT_TRUE(p.error());

  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(cuke::results::test_results().scenarios_count(), 0);
}
TEST_F(parse_error, for_each_scenario_on_an_empty_script)
{
  cuke::parser p;
  p.parse_script("");
  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(cuke::results::test_results().scenarios_count(), 0);
}

// A script that does not parse is a failure of the run. Reporting success
// would let a broken feature file pass unnoticed.
TEST_F(parse_error, a_parse_error_fails_the_run)
{
  EXPECT_EQ(cuke::results::final_result(), cuke::results::test_status::passed);
  cuke::results::test_results().add_parse_error();
  EXPECT_EQ(cuke::results::final_result(), cuke::results::test_status::failed);
}
TEST_F(parse_error, clearing_the_results_clears_the_parse_errors)
{
  cuke::results::test_results().add_parse_error();
  ASSERT_EQ(cuke::results::test_results().parse_errors(), 1);
  cuke::results::test_results().clear();
  EXPECT_EQ(cuke::results::test_results().parse_errors(), 0);
  EXPECT_EQ(cuke::results::final_result(), cuke::results::test_status::passed);
}
