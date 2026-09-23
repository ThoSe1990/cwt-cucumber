#include <cstdint>
#include <filesystem>
#include <format>
#include <random>
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

// A flag right after an optional-value option (--report-json, or either
// --steps-catalog variant) is the next option, not a file path for this
// one: '--report-json --quiet' must not swallow '--quiet' as the report's
// file name and lose the flag.
TEST(options, report_json_before_quiet_does_not_swallow_quiet)
{
  const char* argv[] = {"program", "--report-json", "--quiet"};
  int argc = sizeof(argv) / sizeof(argv[0]);
  cuke::internal::program_args prog_args;
  prog_args.initialize(argc, argv);
  // initialize() acts on the process-wide logger singleton (--quiet here
  // disables it), not just on this local prog_args object; leaving it
  // disabled would silently break whichever test happens to run next.
  cuke::log::enable();

  ASSERT_TRUE(prog_args.is_set(cuke::internal::program_args::arg::report_json));
  EXPECT_TRUE(
      prog_args.get_value(cuke::internal::program_args::arg::report_json)
          .empty());
  EXPECT_TRUE(prog_args.is_set(cuke::internal::program_args::arg::quiet));
}

// Pins the --quiet / --report-json matrix: --quiet decides whether the
// human report (the live run and the final summary) reaches the terminal
// at all; --report-json decides only where the JSON report goes (stdout or
// a named file). Neither is consulted to decide the other, so every
// combination of the two is exercised below.
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
    // Belt and suspenders: report_json_path() already lives outside the
    // repository, but a test that writes it should still not depend on the
    // next test (or the next run) to clean up after it.
    std::error_code ec;
    std::filesystem::remove(report_json_path(), ec);
  }

  // Under the OS temp directory rather than a bare relative name: portable
  // (no POSIX-only assumption) and never lands inside the repository
  // working tree, so running the suite never leaves a stray file for
  // `git status` to notice.
  //
  // The name itself is randomized once per process (a mkstemp-equivalent,
  // in portable standard C++ rather than a platform header): a fixed name
  // in a shared temp directory would collide between two unittests runs on
  // the same machine. Every test in this fixture reuses the same
  // process-wide name, so TearDown() always targets the exact file a test
  // wrote.
  static std::string report_json_path()
  {
    static const std::string unique_name = []
    {
      std::random_device rd;
      std::mt19937_64 gen(rd());
      std::uniform_int_distribution<std::uint64_t> dist;
      return std::format("cwt-cucumber-report-{:016x}.json", dist(gen));
    }();
    return (std::filesystem::temp_directory_path() / unique_name).string();
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
  EXPECT_NE(out.find("Scenario: a scenario"), std::string::npos);
  EXPECT_NE(out.find("1 Scenario ("), std::string::npos);
}

TEST_F(print_results_matrix, quiet_alone_suppresses_everything)
{
  const char* argv[] = {"cucumber", "--quiet"};
  const std::string out = run_and_print(2, argv);
  EXPECT_TRUE(out.empty());
}

// A bare --report-json writes the JSON to stdout, after the run and the
// summary, rather than instead of them.
TEST_F(print_results_matrix, report_json_alone_keeps_the_run_visible)
{
  const char* argv[] = {"cucumber", "--report-json"};
  const std::string out = run_and_print(2, argv);
  const std::size_t summary_pos = out.find("1 Scenario (");
  EXPECT_NE(out.find("Scenario: a scenario"), std::string::npos);
  EXPECT_NE(summary_pos, std::string::npos);
#ifdef WITH_JSON
  const std::size_t json_pos = out.find("\"elements\"");
  EXPECT_NE(json_pos, std::string::npos);
  EXPECT_LT(summary_pos, json_pos);
#endif  // WITH_JSON
}

// --quiet still owns the terminal when --report-json is also set: the JSON
// still reaches stdout (it never goes through the logger), but nothing
// else does.
TEST_F(print_results_matrix, report_json_and_quiet_prints_json_only)
{
  const char* argv[] = {"cucumber", "--report-json", "--quiet"};
  const std::string out = run_and_print(3, argv);
  EXPECT_EQ(out.find("Scenario: a scenario"), std::string::npos);
  EXPECT_EQ(out.find("1 Scenario ("), std::string::npos);
#ifdef WITH_JSON
  EXPECT_NE(out.find("\"elements\""), std::string::npos);
#endif  // WITH_JSON
}

// Naming a file keeps the terminal output the same as a run without
// --report-json; only the JSON's destination changes.
TEST_F(print_results_matrix, report_json_to_file_keeps_the_run_visible)
{
  const std::string path = report_json_path();
  const char* argv[] = {"cucumber", "--report-json", path.c_str()};
  const std::string out = run_and_print(3, argv);
  EXPECT_NE(out.find("Scenario: a scenario"), std::string::npos);
  EXPECT_NE(out.find("1 Scenario ("), std::string::npos);
  EXPECT_EQ(out.find("\"elements\""), std::string::npos);

  ASSERT_TRUE(std::filesystem::exists(path));
#ifdef WITH_JSON
  EXPECT_GT(std::filesystem::file_size(path), 0u);
#endif  // WITH_JSON
}

// --quiet and a named --report-json file combine cleanly: nothing on the
// terminal, the JSON on disk.
TEST_F(print_results_matrix, report_json_to_file_and_quiet_suppresses_terminal)
{
  const std::string path = report_json_path();
  const char* argv[] = {"cucumber", "--report-json", path.c_str(), "--quiet"};
  const std::string out = run_and_print(4, argv);
  EXPECT_TRUE(out.empty());

  ASSERT_TRUE(std::filesystem::exists(path));
#ifdef WITH_JSON
  EXPECT_GT(std::filesystem::file_size(path), 0u);
#endif  // WITH_JSON
}
