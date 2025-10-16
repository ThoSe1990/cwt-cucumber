
#include <gtest/gtest.h>
#include <algorithm>

#include "../src/test_runner.hpp"

#include "test_paths.hpp"

namespace
{
void make_args(std::string_view file_arg, std::string_view tag_expr)
{
  const char* argv[] = {"program", file_arg.data(), "-t", tag_expr.data(),
                        "-v"};
  int argc = sizeof(argv) / sizeof(argv[0]);
  cuke::cuke_args targs;
  targs.initialize(argc, argv);
  [[maybe_unused]] auto& args = cuke::program_arguments(argc, argv);
}
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
    cuke::results::test_results().clear();
    cuke::registry().clear();
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
        "a step"));
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
        "a step with {int} and {string}"));
    cuke::registry().push_hook_before(cuke::internal::hook(
        []() { cuke::internal::get_runtime_options().skip_scenario(true); },
        "@skip"));
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
  ASSERT_EQ(cuke::results::features_back().scenarios.size(), 9);
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
  ASSERT_EQ(scenarios.size(), 1);
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
  ASSERT_TRUE(scenarios.empty());
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::skipped), 0);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::passed), 0);
}
TEST_F(file_io, run_single_scenario_outline_1)
{
  std::string file_arg =
      std::format("{}/test_files/example.feature:21", unittests::test_dir());
  make_args(file_arg);
  cuke::test_runner runner;
  runner.run();

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  auto& scenarios = cuke::results::features_back().scenarios;
  ASSERT_EQ(scenarios.size(), 1);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::skipped), 0);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::passed), 1);
}
TEST_F(file_io, run_single_scenario_outline_2)
{
  std::string file_arg =
      std::format("{}/test_files/example.feature:36", unittests::test_dir());
  make_args(file_arg);
  cuke::test_runner runner;
  runner.run();

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  auto& scenarios = cuke::results::features_back().scenarios;
  ASSERT_EQ(scenarios.size(), 1);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::skipped), 0);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::passed), 1);
}
TEST_F(file_io, run_single_scenario_outline_3)
{
  std::string file_arg =
      std::format("{}/test_files/example.feature:38:36", unittests::test_dir());
  make_args(file_arg);
  cuke::test_runner runner;
  runner.run();

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  auto& scenarios = cuke::results::features_back().scenarios;
  ASSERT_EQ(scenarios.size(), 2);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::skipped), 0);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::passed), 2);
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
  ASSERT_EQ(scenarios.size(), 3);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::skipped), 0);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::passed), 3);
}
TEST_F(file_io, skip_run_all)
{
  std::string file_arg =
      std::format("{}/test_files/skip.feature", unittests::test_dir());
  make_args(file_arg);
  cuke::test_runner runner;
  runner.run();

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  auto& scenarios = cuke::results::features_back().scenarios;
  ASSERT_EQ(scenarios.size(), 2);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::skipped), 1);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::passed), 1);
}
TEST_F(file_io, skip_run_single_scenario)
{
  std::string file_arg =
      std::format("{}/test_files/skip.feature:4", unittests::test_dir());
  make_args(file_arg);
  cuke::test_runner runner;
  runner.run();

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  auto& scenarios = cuke::results::features_back().scenarios;
  ASSERT_EQ(scenarios.size(), 1);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::skipped), 1);
}
TEST_F(file_io, skip_run_single_scenario_w_tag)
{
  std::string file_arg =
      std::format("{}/test_files/skip.feature:9", unittests::test_dir());
  make_args(file_arg, "@tag1");
  cuke::test_runner runner;
  runner.run();

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  auto& scenarios = cuke::results::features_back().scenarios;
  ASSERT_EQ(scenarios.size(), 1);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::passed), 1);
}
TEST_F(file_io, skip_run_all_scenario_w_tag)
{
  std::string file_arg =
      std::format("{}/test_files/skip.feature", unittests::test_dir());
  make_args(file_arg, "@tag1");
  cuke::test_runner runner;
  runner.run();

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  auto& scenarios = cuke::results::features_back().scenarios;
  ASSERT_EQ(scenarios.size(), 1);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::passed), 1);
}
TEST_F(file_io, skip_run_single_scenario_w_tag_no_match)
{
  std::string file_arg =
      std::format("{}/test_files/skip.feature:9", unittests::test_dir());
  make_args(file_arg, "@somethingElse");
  cuke::test_runner runner;
  runner.run();

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  auto& scenarios = cuke::results::features_back().scenarios;
  ASSERT_EQ(scenarios.size(), 0);
}
TEST_F(file_io, skip_run_single_scenario_no_tag)
{
  std::string file_arg =
      std::format("{}/test_files/skip.feature:9", unittests::test_dir());
  make_args(file_arg);
  cuke::test_runner runner;
  runner.run();

  ASSERT_EQ(cuke::results::test_results().data().size(), 1);
  auto& scenarios = cuke::results::features_back().scenarios;
  ASSERT_EQ(scenarios.size(), 1);
  EXPECT_EQ(details::count(scenarios, cuke::results::test_status::passed), 1);
}
