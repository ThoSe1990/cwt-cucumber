#include <gtest/gtest.h>
#include <stdexcept>

#include "../src/test_runner.hpp"
#include "../src/parser.hpp"
#include "../src/asserts.hpp"

#include "test_paths.hpp"

class stdout_print : public ::testing::Test
{
 protected:
  void TearDown() override
  {
    auto& args = cuke::program_arguments(0, {});
    args.clear();
  }
  void SetUp() override
  {
    testing::internal::CaptureStdout();

    cuke::results::test_results().clear();
    cuke::registry().clear();
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
        "a step"));
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&)
        { cuke::is_true(false); }, "this fails"));
    cuke::registry().push_step(
        cuke::internal::step_definition([](const cuke::value_array& values, const auto&,
                                const auto&, const auto&) {},
                             "a step with {int} and {string}"));
    cuke::registry().push_step(
        cuke::internal::step_definition([](const cuke::value_array& values, const auto&,
                                const auto&, const auto&) {},
                             "a step with a table"));
  }

  [[nodiscard]] bool has_substr(const std::string& output,
                                const std::string& expected) const noexcept
  {
    return output.find(expected) != std::string::npos;
  }
};

TEST_F(stdout_print, scenario_pass)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: a scenario 
    Given a step 
  )*";

  cuke::parser p;
  p.parse_script(script);

  cuke::test_runner runner;
  p.for_each_scenario(runner);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(has_substr(output, "Feature: a feature"));
  EXPECT_TRUE(has_substr(output, "<no file>:2"));
  EXPECT_TRUE(has_substr(output, "Scenario: a scenario"));
  EXPECT_TRUE(has_substr(output, "<no file>:3"));
  EXPECT_TRUE(has_substr(output, "[   PASSED    ] Given a step"));
}
TEST_F(stdout_print, scenario_fail)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: a scenario 
    Given a step 
    And this fails
  )*";

  cuke::parser p;
  p.parse_script(script);

  cuke::test_runner runner;
  p.for_each_scenario(runner);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(has_substr(output, "[   PASSED    ] Given a step"));
  EXPECT_TRUE(has_substr(output, "<no file>:4"));
  EXPECT_TRUE(has_substr(output, "[   FAILED    ] And this fails"));
  EXPECT_TRUE(has_substr(output, "<no file>:5"));
}
TEST_F(stdout_print, scenario_undefined)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: a scenario 
    Given an undefined step 
    And something else
  )*";

  cuke::parser p;
  p.parse_script(script);

  cuke::test_runner runner;
  p.for_each_scenario(runner);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(has_substr(output, "[   UNDEFINED ] Given an undefined step"));
  EXPECT_TRUE(has_substr(output, "<no file>:4"));
  EXPECT_TRUE(has_substr(output, "[   SKIPPED   ] And something else"));
  EXPECT_TRUE(has_substr(output, "<no file>:5"));
}
TEST_F(stdout_print, scenario_outline)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario Outline: a scenario outline
    Given a step with <var 1> and <var 2> 

    Examples: 
    | var 1 | var 2       |
    | 123   | "some text" |

  )*";

  cuke::parser p;
  p.parse_script(script);

  cuke::test_runner runner;
  p.for_each_scenario(runner);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(has_substr(output, "Scenario Outline: a scenario outline"));
  EXPECT_TRUE(has_substr(output, "<no file>:3"));
  EXPECT_TRUE(has_substr(
      output, "[   PASSED    ] Given a step with 123 and \"some text\""));
  EXPECT_TRUE(has_substr(output, "<no file>:4"));
}
TEST_F(stdout_print, scenario_outline_datatable)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario Outline: a scenario outline
    Given a step with a table 
    | 1 | 2 | <var 1> | <var 2> |

    Examples: 
    | var 1 | var 2       |
    | 123   | "some text" |

  )*";

  cuke::parser p;
  p.parse_script(script);

  cuke::test_runner runner;
  p.for_each_scenario(runner);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(has_substr(output, "Scenario Outline: a scenario outline"));
  EXPECT_TRUE(has_substr(output, "<no file>:3"));
  EXPECT_TRUE(has_substr(
      output, "[   PASSED    ] Given a step with a table"));
  EXPECT_TRUE(has_substr(output, "| 1 | 2 | 123 | \"some text\" |"));
  EXPECT_TRUE(has_substr(output, "<no file>:4"));
}
TEST_F(stdout_print, scenario_from_file)
{
  std::string file_arg =
      std::format("{}/test_files/example.feature:3", unittests::test_dir());
  const char* argv[] = {"program", file_arg.c_str()};
  int argc = sizeof(argv) / sizeof(argv[0]);
  cuke::cuke_args targs;
  targs.initialize(argc, argv);

  const cuke::feature_file& file = targs.get_options().files.back();

  cuke::parser p;
  p.parse_from_file(file);

  cuke::test_runner runner;
  p.for_each_scenario(runner);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(has_substr(output, "Feature: example feature"));
  EXPECT_TRUE(has_substr(output, "Scenario: first scenario"));
  EXPECT_TRUE(has_substr(output, "[   PASSED    ] Given a step"));
  EXPECT_TRUE(has_substr(output, "[   PASSED    ] And a step"));
}
TEST_F(stdout_print, final_result_1)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: a scenario 
    Given a step 
  )*";

  cuke::parser p;
  p.parse_script(script);

  cuke::test_runner runner;
  p.for_each_scenario(runner);

  [[maybe_unused]] std::string output = testing::internal::GetCapturedStdout();

  std::string scenario_result = cuke::results::scenarios_to_string();
  EXPECT_TRUE(has_substr(scenario_result, "1 Scenario"));
  EXPECT_TRUE(has_substr(scenario_result, "1 passed"));

  std::string steps_result = cuke::results::steps_to_string();
  EXPECT_TRUE(has_substr(steps_result, "1 Step"));
  EXPECT_TRUE(has_substr(steps_result, "1 passed"));
}
TEST_F(stdout_print, final_result_2)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: a scenario 
    Given a step 
    Given a step 
    Given a step 

    Scenario: a scenario 
    Given a step 
    Given a step 
  )*";

  cuke::parser p;
  p.parse_script(script);

  cuke::test_runner runner;
  p.for_each_scenario(runner);

  [[maybe_unused]] std::string output = testing::internal::GetCapturedStdout();

  std::string scenario_result = cuke::results::scenarios_to_string();
  EXPECT_TRUE(has_substr(scenario_result, "2 Scenarios"));
  EXPECT_TRUE(has_substr(scenario_result, "2 passed"));

  std::string steps_result = cuke::results::steps_to_string();
  EXPECT_TRUE(has_substr(steps_result, "5 Steps"));
  EXPECT_TRUE(has_substr(steps_result, "5 passed"));
}
TEST_F(stdout_print, final_result_3)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: a scenario 
    Given a step 
   
    Scenario: a scenario
    Given a step
    Given an undefined step 
    And something else
  )*";

  cuke::parser p;
  p.parse_script(script);

  cuke::test_runner runner;
  p.for_each_scenario(runner);

  [[maybe_unused]] std::string output = testing::internal::GetCapturedStdout();

  std::string scenario_result = cuke::results::scenarios_to_string();
  EXPECT_TRUE(has_substr(scenario_result, "2 Scenarios"));
  EXPECT_TRUE(has_substr(scenario_result, "1 failed"));
  EXPECT_TRUE(has_substr(scenario_result, "1 passed"));

  std::string steps_result = cuke::results::steps_to_string();
  EXPECT_TRUE(has_substr(steps_result, "4 Steps"));
  EXPECT_TRUE(has_substr(steps_result, "1 undefined"));
  EXPECT_TRUE(has_substr(steps_result, "1 skipped"));
  EXPECT_TRUE(has_substr(steps_result, "2 passed"));
}

TEST_F(stdout_print, final_result_4)
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: a scenario 
    Given a step 
    And this fails
  )*";

  cuke::parser p;
  p.parse_script(script);

  cuke::test_runner runner;
  p.for_each_scenario(runner);

  [[maybe_unused]] std::string output = testing::internal::GetCapturedStdout();

  std::string scenario_result = cuke::results::scenarios_to_string();
  EXPECT_TRUE(has_substr(scenario_result, "1 Scenario"));
  EXPECT_TRUE(has_substr(scenario_result, "1 failed"));

  std::string steps_result = cuke::results::steps_to_string();
  EXPECT_TRUE(has_substr(steps_result, "2 Steps"));
  EXPECT_TRUE(has_substr(steps_result, "1 failed"));
  EXPECT_TRUE(has_substr(steps_result, "1 passed"));
}

namespace cuke
{
extern void print_failed_scenarios();
}

TEST_F(stdout_print, scenario_fail_final_form_file)
{
  std::string file_arg =
      std::format("{}/test_files/fail.feature:3", unittests::test_dir());
  const char* argv[] = {"program", file_arg.c_str()};
  int argc = sizeof(argv) / sizeof(argv[0]);

  cuke::cuke_args& targs = cuke::program_arguments(argc, argv);

  const cuke::feature_file& file = targs.get_options().files.back();

  cuke::parser p;
  p.parse_from_file(file);

  cuke::test_runner runner;
  p.for_each_scenario(runner);

  cuke::print_failed_scenarios();
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(has_substr(output, "Failed Scenarios:"));
  EXPECT_TRUE(has_substr(output, "first scenario"));
  EXPECT_TRUE(has_substr(output, "fail.feature:3"));
}

TEST_F(stdout_print, scenario_fail_final_form_file_quiet)
{
  // TODO:
  // --quiet option suppresses the print in assert.hpp
  // but unfortunatelly we have to call set_quiet() on test runner
  // manually, lets refactor the test runner later ...
  std::string file_arg =
      std::format("{}/test_files/fail.feature:3", unittests::test_dir());
  const char* argv[] = {"program", file_arg.c_str(), "--quiet"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  cuke::cuke_args& targs = cuke::program_arguments(argc, argv);
  const cuke::feature_file& file = targs.get_options().files.back();

  cuke::parser p;
  p.parse_from_file(file);

  cuke::test_runner runner;
  p.for_each_scenario(runner);

  cuke::print_failed_scenarios();
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_FALSE(has_substr(output, "[   PASSED    ] Given a step"));
  EXPECT_FALSE(
      has_substr(output, "Expected given condition true, but its false:"));
  EXPECT_FALSE(has_substr(output, "[   FAILED    ] And this fails"));
}
TEST_F(stdout_print, scenario_fail_final_form_file_q)
{
  // TODO:
  // --quiet option suppresses the print in assert.hpp
  // but unfortunatelly we have to call set_quiet() on test runner
  // manually, lets refactor the test runner later ...
  std::string file_arg =
      std::format("{}/test_files/fail.feature:3", unittests::test_dir());
  const char* argv[] = {"program", file_arg.c_str(), "-q"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  cuke::cuke_args& targs = cuke::program_arguments(argc, argv);
  const cuke::feature_file& file = targs.get_options().files.back();

  cuke::parser p;
  p.parse_from_file(file);

  cuke::test_runner runner;
  p.for_each_scenario(runner);

  cuke::print_failed_scenarios();
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_FALSE(has_substr(output, "[   PASSED    ] Given a step"));
  EXPECT_FALSE(
      has_substr(output, "Expected given condition true, but its false:"));
  EXPECT_FALSE(has_substr(output, "[   FAILED    ] And this fails"));
}
