#include <format>
#include <gtest/gtest.h>

#include "../src/ast/test_runner.hpp"
#include "../src/ast/parser.hpp"
#include "../src/asserts.hpp"

#include "paths.hpp"

class stdout_print : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    testing::internal::CaptureStdout();

    cuke::results::test_results().clear();
    cuke::registry().clear();
    cuke::registry().push_step(
        cuke::internal::step([](const cuke::value_array&) {}, "a step"));
    cuke::registry().push_step(cuke::internal::step(
        [](const cuke::value_array&) { cuke::is_true(false); }, "this fails"));
  }

  [[nodiscard]] bool has_substr(const std::string& output,
                                const std::string& expected) const noexcept
  {
    return output.find(expected) != std::string::npos;
  }
};

TEST_F(stdout_print, scenario_from_file)
{
  std::string file_arg =
      std::format("{}/test_files/example.feature:3", unittests::test_dir());
  const char* argv[] = {"program", file_arg.c_str()};
  int argc = sizeof(argv) / sizeof(argv[0]);
  cuke::internal::terminal_arguments targs(argc, argv);

  const cuke::internal::feature_file& file = targs.get_options().files.back();

  cuke::parser p;
  p.parse_from_file(file);

  cuke::test_runner runner(&file);
  p.for_each_scenario(runner);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(has_substr(output, "Feature: example feature"));
  EXPECT_TRUE(has_substr(output, "Scenario: first scenario"));
  EXPECT_TRUE(has_substr(output, "[   PASSED    ] Given a step"));
  EXPECT_TRUE(has_substr(output, "[   PASSED    ] And a step"));
}

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
  EXPECT_TRUE(has_substr(output, "<no file>:4"));
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
