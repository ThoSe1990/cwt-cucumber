
#include <gtest/gtest.h>

#include "../src/ast/test_runner.hpp"
#include "../src/ast/parser.hpp"
#include "paths.hpp"

class file_io : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    cuke::registry().clear();
    cuke::registry().push_step(
        cuke::internal::step([](const cuke::value_array&) {}, "a step"));
    cuke::registry().push_step(cuke::internal::step(
        [](const cuke::value_array&) {}, "a step with {int} and {string}"));
  }
};

TEST_F(file_io, run_scenario)
{
  cuke::parser p;
  p.parse_from_file(
      std::format("{}/test_files/example.feature", unittests::test_dir()));
  cuke::test_runner runner;
  p.for_each_scenario(runner);
  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  ASSERT_EQ(cuke::results::features_back().scenarios.size(), 7);
  EXPECT_EQ(cuke::results::features_back().status,
            cuke::results::test_status::passed);
  EXPECT_EQ(cuke::results::scenarios_back().status,
            cuke::results::test_status::passed);
}
