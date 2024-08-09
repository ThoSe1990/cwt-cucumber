
#include <gtest/gtest.h>

#include "../src/ast/test_runner.hpp"
#include "../src/ast/parser.hpp"
#include "paths.hpp"

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
}

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
