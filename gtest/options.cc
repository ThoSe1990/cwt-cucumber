#include <format>
#include <string_view>

#include <gtest/gtest.h>

#include "../src/options.hpp"
#include "../src/tags.hpp"
#include "../src/log.hpp"
#include "../src/parser.hpp"
#include "../src/test_runner.hpp"
#include "../src/test_results.hpp"
#include "../src/cucumber.hpp"

#include "test_paths.hpp"

TEST(options, file_path_doesnt_exist)
{
  const char* argv[] = {"program", "path/doesnt/exist/to/file.feature"};
  int argc = sizeof(argv) / sizeof(argv[0]);
  cuke::internal::program_args prog_args;
  prog_args.initialize(argc, argv);
  ASSERT_TRUE(prog_args.get_feature_files().empty());
  ASSERT_TRUE(prog_args.get_excluded_files().empty());
}
TEST(options, file_path_does_exist)
{
  std::string path =
      std::format("{}/test_files/any.feature", unittests::test_dir());
  const char* argv[] = {"program", path.c_str()};
  int argc = sizeof(argv) / sizeof(argv[0]);
  cuke::internal::program_args prog_args;
  prog_args.initialize(argc, argv);
  ASSERT_FALSE(prog_args.get_feature_files().empty());
  EXPECT_EQ(prog_args.get_feature_files().at(0).path, std::string(argv[1]));
}

namespace details
{
[[nodiscard]] bool has_file(
    const std::vector<cuke::internal::feature_file>& container,
    std::string_view file_name)
{
  for (const cuke::internal::feature_file& file : container)
  {
    if (file.path.ends_with(file_name))
    {
      return true;
    }
  }
  return false;
}
}  // namespace details

TEST(options, find_files_in_dir)
{
  std::string path = std::format("{}/test_files", unittests::test_dir());
  const char* argv[] = {"program", path.c_str()};
  int argc = sizeof(argv) / sizeof(argv[0]);
  cuke::internal::program_args prog_args;
  prog_args.initialize(argc, argv);
  ASSERT_EQ(prog_args.get_feature_files().size(), 4);

  EXPECT_TRUE(details::has_file(prog_args.get_feature_files(), "any.feature"));
  EXPECT_TRUE(
      details::has_file(prog_args.get_feature_files(), "example.feature"));
  EXPECT_TRUE(details::has_file(prog_args.get_feature_files(), "fail.feature"));
  EXPECT_TRUE(details::has_file(prog_args.get_feature_files(), "skip.feature"));
}
namespace details
{
static std::string remove_trailing_char(std::string_view str, std::size_t n)
{
  return std::string(str.substr(0, str.size() - n));
}
}  // namespace details
TEST(options, file_path_does_exist_w_line)
{
  std::string path =
      std::format("{}/test_files/any.feature:3", unittests::test_dir());
  const char* argv[] = {"program", path.c_str()};
  int argc = sizeof(argv) / sizeof(argv[0]);
  cuke::internal::program_args prog_args;
  prog_args.initialize(argc, argv);
  ASSERT_FALSE(prog_args.get_feature_files().empty());
  EXPECT_EQ(prog_args.get_feature_files().at(0).path,
            details::remove_trailing_char(argv[1], 2));
  ASSERT_FALSE(prog_args.get_feature_files().at(0).lines_to_run.empty());
  EXPECT_TRUE(prog_args.get_feature_files().at(0).lines_to_run.contains(3));
}
TEST(options, file_path_does_exist_w_lines)
{
  std::string path = std::format("{}/test_files/any.feature:3:123:9999",
                                 unittests::test_dir());
  const char* argv[] = {"program", path.c_str()};
  int argc = sizeof(argv) / sizeof(argv[0]);
  cuke::internal::program_args prog_args;
  prog_args.initialize(argc, argv);
  ASSERT_FALSE(prog_args.get_feature_files().empty());
  EXPECT_EQ(prog_args.get_feature_files().at(0).path,
            details::remove_trailing_char(argv[1], 11));
  ASSERT_EQ(prog_args.get_feature_files().at(0).lines_to_run.size(), 3);
  EXPECT_TRUE(prog_args.get_feature_files().at(0).lines_to_run.contains(3));
  EXPECT_TRUE(prog_args.get_feature_files().at(0).lines_to_run.contains(123));
  EXPECT_TRUE(prog_args.get_feature_files().at(0).lines_to_run.contains(9999));
}
TEST(options, tag_expression_1)
{
  const char* argv[] = {"program", "-t", "@tag1 or @tag2"};
  int argc = sizeof(argv) / sizeof(argv[0]);
  cuke::internal::program_args prog_args;
  prog_args.initialize(argc, argv);
  ASSERT_TRUE(prog_args.is_set(cuke::internal::program_args::arg::tags));
  ASSERT_FALSE(
      prog_args.get_value(cuke::internal::program_args::arg::tags).empty());

  cuke::internal::tag_expression tags(
      prog_args.get_value(cuke::internal::program_args::arg::tags));
  EXPECT_TRUE(tags.evaluate(std::vector{std::string{"@tag1"}}));
  EXPECT_TRUE(tags.evaluate(std::vector{std::string{"@tag2"}}));
  EXPECT_FALSE(tags.evaluate(std::vector{std::string{"@tag3"}}));
}
TEST(options, tag_expression_2)
{
  const char* argv[] = {"program", "--tags", "@tag1 or @tag2"};
  int argc = sizeof(argv) / sizeof(argv[0]);
  cuke::internal::program_args prog_args;
  prog_args.initialize(argc, argv);
  ASSERT_TRUE(prog_args.is_set(cuke::internal::program_args::arg::tags));
  ASSERT_FALSE(
      prog_args.get_value(cuke::internal::program_args::arg::tags).empty());

  cuke::internal::tag_expression tags(
      prog_args.get_value(cuke::internal::program_args::arg::tags));

  EXPECT_TRUE(tags.evaluate(std::vector{std::string{"@tag1"}}));
  EXPECT_TRUE(tags.evaluate(std::vector{std::string{"@tag2"}}));
  EXPECT_FALSE(tags.evaluate(std::vector{std::string{"@tag3"}}));
}

// Pins the terminal half of the --quiet / --report-json matrix: --quiet
// decides whether the human report (the live run and the final summary)
// reaches the terminal at all, and nothing else. --report-json is covered
// separately below; here it is always unset.
class print_results_matrix : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    cuke::registry().clear();
    cuke::results::test_results().clear();
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
        "a step"));
  }
  void TearDown() override
  {
    cuke::internal::get_program_args(0, {}).clear();
    cuke::log::enable();
  }

  // Runs one scenario the way the CLI does: the live per-step lines from
  // the run itself, immediately followed by print_results()'s summary (and
  // JSON, once --report-json is in play), captured as a single block of
  // stdout so the assertions below see exactly what a terminal would.
  static std::string run_and_print(int argc, const char* argv[])
  {
    [[maybe_unused]] auto& args = cuke::internal::get_program_args(argc, argv);

    const char* script = R"*(
      Feature: a feature
      Scenario: a scenario
      Given a step
    )*";
    cuke::parser p;
    p.parse_script(script);

    testing::internal::CaptureStdout();
    cuke::test_runner runner;
    p.for_each_scenario(runner);

    cuke::cwt_cucumber cucumber(argc, argv);
    cucumber.print_results();
    return testing::internal::GetCapturedStdout();
  }
};

TEST_F(print_results_matrix, no_options_show_the_run_and_the_summary)
{
  const char* argv[] = {"cucumber"};
  const std::string out = run_and_print(1, argv);
  EXPECT_NE(out.find("a scenario"), std::string::npos);
  EXPECT_NE(out.find("1 Scenario ("), std::string::npos);
}

TEST_F(print_results_matrix, quiet_alone_suppresses_everything)
{
  const char* argv[] = {"cucumber", "--quiet"};
  const std::string out = run_and_print(2, argv);
  EXPECT_TRUE(out.empty());
}
