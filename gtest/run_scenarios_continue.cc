#include <gtest/gtest.h>

#include "../src/registry.hpp"
#include "../src/options.hpp"
#include "../src/parser.hpp"
#include "../src/test_runner.hpp"
#include "../src/asserts.hpp"
#include "../src/test_results.hpp"

namespace
{
void make_args(std::string_view option)
{
  const char* argv[] = {"program", option.data()};
  int argc = sizeof(argv) / sizeof(argv[0]);
  [[maybe_unused]] auto& args = cuke::get_program_args(argc, argv);
}

bool has_skipped_steps(const cuke::results::feature& feature)
{
  for (const cuke::results::scenario& scenario : feature.scenarios)
  {
    for (const cuke::results::step& step : scenario.steps)
    {
      if (step.status == cuke::results::test_status::skipped)
      {
        return true;
      }
    }
  }
  return false;
}

}  // namespace

class run_scenarios_continue : public ::testing::Test
{
 protected:
  void TearDown() override
  {
    [[maybe_unused]] auto& args = cuke::get_program_args(0, {});
    args.clear();
    call_count = 0;
  }
  void SetUp() override
  {
    cuke::registry().clear();
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&)
        { run_scenarios_continue::call_count++; }, "a step"));
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&)
        {
          run_scenarios_continue::call_count++;
          cuke::is_true(false);
        },
        "this fails"));
  }

  static std::size_t call_count;
};

std::size_t run_scenarios_continue::call_count = 0;

TEST_F(run_scenarios_continue, continue_on_fail_1)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: a scenario 
    * a step 
    * this fails
    * a step 
  )*";
  cuke::parser p;
  p.parse_script(script);

  make_args("--continue-on-failure");

  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenarios_continue::call_count, 3);

  const auto& scenario = cuke::results::scenarios_back();
  EXPECT_EQ(scenario.status, cuke::results::test_status::failed);

  const auto& feature = cuke::results::features_back();
  EXPECT_FALSE(has_skipped_steps(feature));
}
TEST_F(run_scenarios_continue, continue_on_fail_2)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: a scenario 
    * a step 
    * this fails
    * a step 
  )*";
  cuke::parser p;
  p.parse_script(script);

  make_args("-c");

  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenarios_continue::call_count, 3);

  const auto& scenario = cuke::results::scenarios_back();
  EXPECT_EQ(scenario.status, cuke::results::test_status::failed);

  const auto& feature = cuke::results::features_back();
  EXPECT_FALSE(has_skipped_steps(feature));
}
TEST_F(run_scenarios_continue, continue_on_fail_3)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: a scenario 
    * this fails
    * a step 
    
    Scenario: a scenario 
    * a step 
  )*";
  cuke::parser p;
  p.parse_script(script);

  make_args("-c");

  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenarios_continue::call_count, 3);

  const auto& scenario = cuke::results::scenarios_back();
  EXPECT_EQ(scenario.status, cuke::results::test_status::passed);

  const auto& feature = cuke::results::features_back();
  EXPECT_FALSE(has_skipped_steps(feature));
}
TEST_F(run_scenarios_continue, continue_on_fail_4)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: a scenario 
    * this is undefined ...  
    * a step  
  )*";
  cuke::parser p;
  p.parse_script(script);

  make_args("-c");

  cuke::test_runner runner;
  p.for_each_scenario(runner);

  EXPECT_EQ(run_scenarios_continue::call_count, 1);

  const auto& scenario = cuke::results::scenarios_back();
  EXPECT_EQ(scenario.status, cuke::results::test_status::failed);

  const auto& feature = cuke::results::features_back();
  EXPECT_FALSE(has_skipped_steps(feature));
}
