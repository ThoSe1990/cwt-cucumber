
#include <gtest/gtest.h>
#include <algorithm>

#include "../src/test_runner.hpp"

#include "test_paths.hpp"

namespace
{
void make_args(std::string_view file_arg)
{
  const char* argv[] = {"program", file_arg.data()};
  int argc = sizeof(argv) / sizeof(argv[0]);
  cuke::cuke_args targs;
  targs.initialize(argc, argv);
  [[maybe_unused]] auto& args = cuke::program_arguments(argc, argv);
}
}  // namespace

class file_io : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
        "a step"));
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
        "a step with {int} and {string}"));
  }
  void TearDown() override
  {
    cuke::registry().clear();
    cuke::results::test_results().clear();
    auto& args = cuke::program_arguments(0, {});
    args.clear();
  }
};

TEST_F(file_io, run_scenario)
{
  make_args(
      std::format("{}/test_files/example.feature", unittests::test_dir()));
  cuke::test_runner runner;
  runner.run();
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
  make_args(
      std::format("{}/test_files/example.feature:3", unittests::test_dir()));
  cuke::test_runner runner;
  runner.run();

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  auto& scenarios = cuke::results::features_back().scenarios;
  ASSERT_EQ(scenarios.size(), 7);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::skipped), 6);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::passed), 1);
}
TEST_F(file_io, run_single_scenario_wrong_line)
{
  make_args(
      std::format("{}/test_files/example.feature:4", unittests::test_dir()));
  cuke::test_runner runner;
  runner.run();

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
  make_args(file_arg);
  cuke::test_runner runner;
  runner.run();

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
  make_args(file_arg);
  cuke::test_runner runner;
  runner.run();

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  auto& scenarios = cuke::results::features_back().scenarios;
  ASSERT_EQ(scenarios.size(), 7);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::skipped), 4);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::passed), 3);
}
