
#include <gtest/gtest.h>
#include <algorithm>

#include "../src/test_runner.hpp"
#include "../src/parser.hpp"
#include "paths.hpp"

class file_io : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    cuke::registry().push_step(
        cuke::internal::step([](const cuke::value_array&) {}, "a step"));
    cuke::registry().push_step(cuke::internal::step(
        [](const cuke::value_array&) {}, "a step with {int} and {string}"));
  }
  void TearDown() override
  {
    cuke::registry().clear();
    cuke::results::test_results().clear();
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

namespace details
{
template <typename T>
[[nodiscard]] static std::size_t count(const std::vector<T>& results,
                                       cuke::results::test_status status)
{
  return std::count_if(results.begin(), results.end(),
                       [&status](const auto& s) { return s.status == status; });
}
}  // namespace details

TEST_F(file_io, run_single_scenario)
{
  std::string file_arg =
      std::format("{}/test_files/example.feature:3", unittests::test_dir());
  const char* argv[] = {"program", file_arg.c_str()};
  int argc = sizeof(argv) / sizeof(argv[0]);
  cuke::internal::terminal_arguments targs(argc, argv);

  const cuke::internal::feature_file& file = targs.get_options().files.back();

  cuke::parser p;
  p.parse_from_file(file.path);

  cuke::test_runner runner(&file);
  p.for_each_scenario(runner);

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  auto& scenarios = cuke::results::features_back().scenarios;
  ASSERT_EQ(scenarios.size(), 7);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::skipped), 6);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::passed), 1);
}
TEST_F(file_io, run_single_scenario_wrong_line)
{
  std::string file_arg =
      std::format("{}/test_files/example.feature:4", unittests::test_dir());
  const char* argv[] = {"program", file_arg.c_str()};
  int argc = sizeof(argv) / sizeof(argv[0]);
  cuke::internal::terminal_arguments targs(argc, argv);

  const cuke::internal::feature_file& file = targs.get_options().files.back();

  cuke::parser p;
  p.parse_from_file(file.path);

  cuke::test_runner runner(&file);
  p.for_each_scenario(runner);

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  auto& scenarios = cuke::results::features_back().scenarios;
  ASSERT_EQ(scenarios.size(), 7);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::skipped), 7);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::passed), 0);
}
TEST_F(file_io, run_single_scenario_outline)
{
  std::string file_arg =
      std::format("{}/test_files/example.feature:21", unittests::test_dir());
  const char* argv[] = {"program", file_arg.c_str()};
  int argc = sizeof(argv) / sizeof(argv[0]);
  cuke::internal::terminal_arguments targs(argc, argv);

  const cuke::internal::feature_file& file = targs.get_options().files.back();

  cuke::parser p;
  p.parse_from_file(file.path);

  cuke::test_runner runner(&file);
  p.for_each_scenario(runner);

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  auto& scenarios = cuke::results::features_back().scenarios;
  ASSERT_EQ(scenarios.size(), 7);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::skipped), 6);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::passed), 1);
}
TEST_F(file_io, run_multiple_scenario)
{
  std::string file_arg = std::format("{}/test_files/example.feature:3:21:11",
                                     unittests::test_dir());
  const char* argv[] = {"program", file_arg.c_str()};
  int argc = sizeof(argv) / sizeof(argv[0]);
  cuke::internal::terminal_arguments targs(argc, argv);

  const cuke::internal::feature_file& file = targs.get_options().files.back();

  cuke::parser p;
  p.parse_from_file(file.path);

  cuke::test_runner runner(&file);
  p.for_each_scenario(runner);

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  auto& scenarios = cuke::results::features_back().scenarios;
  ASSERT_EQ(scenarios.size(), 7);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::skipped), 4);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::passed), 3);
}
