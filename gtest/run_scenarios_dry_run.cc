#include <gtest/gtest.h>
#include <algorithm>

#include "../src/test_runner.hpp"
#include "../src/parser.hpp"
#include "test_results.hpp"

namespace
{
void make_args(std::string_view option)
{
  const char* argv[] = {"program", option.data()};
  int argc = sizeof(argv) / sizeof(argv[0]);
  [[maybe_unused]] auto& args = cuke::get_program_args(argc, argv);
}
}  // namespace

class run_scenarios_dry_run : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    call_count = 0;
    cuke::results::test_results().clear();
    cuke::registry().clear();
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&)
        { run_scenarios_dry_run::call_count++; }, "a step"));
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&)
        { run_scenarios_dry_run::call_count++; }, "a step with an {int}"));
  }

  static std::size_t call_count;
};
std::size_t run_scenarios_dry_run::call_count = 0;

TEST_F(run_scenarios_dry_run, run_scenario_dry_long_option)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given a step 
    Scenario: Second Scenario 
    Given a step 
    Given a step 
  )*";

  make_args("--dry-run");

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_dry_run::call_count, 0);

  using namespace cuke::results;
  EXPECT_EQ(final_result(), test_status::passed);

  const auto& scenarios = features_back().scenarios;
  EXPECT_TRUE(std::all_of(scenarios.begin(), scenarios.end(),
                          [](const auto& scenario)
                          { return scenario.status == test_status::skipped; }));
}

TEST_F(run_scenarios_dry_run, run_scenario_dry_short_option)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given a step 
    Scenario: Second Scenario 
    Given a step 
    Given a step 
  )*";

  make_args("-d");

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_dry_run::call_count, 0);

  using namespace cuke::results;
  EXPECT_EQ(final_result(), test_status::passed);

  const auto& scenarios = features_back().scenarios;
  EXPECT_TRUE(std::all_of(scenarios.begin(), scenarios.end(),
                          [](const auto& scenario)
                          { return scenario.status == test_status::skipped; }));
}
TEST_F(run_scenarios_dry_run, run_scenario_outline_dry_short_option)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given a step 
    Scenario Outline: Scenario Outline
    Given a step 
    And a step with an <integer>

    Examples:
    | integer | 
    | 123     |
    | 456     |
  )*";

  make_args("-d");

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_dry_run::call_count, 0);

  using namespace cuke::results;
  EXPECT_EQ(final_result(), test_status::passed);

  const auto& scenarios = features_back().scenarios;
  EXPECT_TRUE(std::all_of(scenarios.begin(), scenarios.end(),
                          [](const auto& scenario)
                          { return scenario.status == test_status::skipped; }));
}
TEST_F(run_scenarios_dry_run, run_scenario_dry_w_undefined_step)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given a step 
    And an undefined step
    Scenario: Second Scenario 
    Given a step 
    Given a step 
  )*";

  make_args("-d");

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_dry_run::call_count, 0);

#ifdef UNDEFINED_STEPS_ARE_A_FAILURE
  using namespace cuke::results;
  EXPECT_EQ(final_result(), test_status::failed);

  const auto& scenarios = features_back().scenarios;
  EXPECT_EQ(scenarios.at(0).status, test_status::failed);
#else
  using namespace cuke::results;
  const auto& scenarios = features_back().scenarios;
  EXPECT_EQ(scenarios.at(0).status, test_status::skipped);

  using namespace cuke::results;
  EXPECT_EQ(final_result(), test_status::passed);
#endif
}
TEST_F(run_scenarios_dry_run, run_scenario_outline_dry_w_undefined_step)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario Outline: Scenario Outline
    Given a step 
    And a step with an <integer>
    And something undefined 

    Examples:
    | integer | 
    | 123     |
    | 456     |
  )*";

  make_args("-d");

  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);
  EXPECT_EQ(run_scenarios_dry_run::call_count, 0);

#ifdef UNDEFINED_STEPS_ARE_A_FAILURE
  using namespace cuke::results;
  EXPECT_EQ(final_result(), test_status::failed);

  const auto& scenarios = features_back().scenarios;
  EXPECT_EQ(scenarios.at(0).status, test_status::failed);
  EXPECT_EQ(scenarios.at(1).status, test_status::failed);
#else
  using namespace cuke::results;
  EXPECT_EQ(final_result(), test_status::passed);

  const auto& scenarios = features_back().scenarios;
  EXPECT_EQ(scenarios.at(0).status, test_status::skipped);
  EXPECT_EQ(scenarios.at(1).status, test_status::skipped);
#endif
}
