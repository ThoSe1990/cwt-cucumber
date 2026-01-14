#pragma once

#include <cstdint>
#include <chrono>
#include <optional>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <filesystem>

#include "log.hpp"
#include "version.hpp"

namespace cuke
{
/**
 * @brief Sets the current scenario to be skipped.
 *
 * This function sets the runtime option to skip the currently executing
 * scenario. The scenario will not be executed, but it will appear in the
 * test report as skipped. After applying this option, the runtime will
 * automatically reset it.
 */
void skip_scenario();

/**
 * @brief Ignores the current scenario.
 *
 * This function sets the runtime option to ignore the currently executing
 * scenario. The scenario will not be executed and not be reported. After
 * applying this option, the runtime will automatically reset it.
 */
void ignore_scenario();

/**
 * @brief Sets the current scenario as failed.
 *
 * This function sets the runtime option to fail the currently executing
 * scenario. After applying this option, the runtime will automatically
 * reset it.
 *
 * @param msg Optional message describing the reason for failure.
 */
void fail_scenario(const std::string_view msg = "");

/**
 * @brief Sets the current step as failed.
 *
 * This function sets the runtime option to fail the currently executing step.
 *
 * @param msg Optional message describing the reason for failure. Default is an
 * empty string. After applying this option, the runtime will automatically
 * reset it.
 */
void fail_step(const std::string_view msg = "");

struct feature_file
{
  std::string path;
  std::unordered_set<std::size_t> lines_to_run;
};

}  // namespace cuke

namespace cuke
{

static constexpr const char* tag_description =
    R"(Provide a tag expression to execute only Features/Scenarios with given tags
          Examples:
            "@tag1"
            "@tag1 or @tag2"
            "not @tag2"
            "(@tag1 and @tag2) or not @tag3"
            "((@tag1 and @tag2) or @tag3) xor @tag4"
)";

struct options
{
};

class program_args
{
 public:
  enum class arg : int8_t
  {
    none = 0,
    help,
    quiet,
    dry_run,
    verbose,
    continue_on_failure,
    report_json,
    steps_catalog_readable,
    steps_catalog_json,
    exclude_file,
    tags,
    name_filter,
  };
  enum class arg_type : int8_t
  {
    option = 0,
    flag,
    exclude_file,
  };

  struct definition
  {
    std::string_view short_key;
    std::string_view long_key;
    std::string_view value;
    arg key;
    arg_type type;
    std::string_view description;
  };
  struct info
  {
    arg key;
    arg_type type;
    std::string_view description;
  };
  // clang-format off
  static constexpr const definition defs[] = {
      {"-h", "--help", "", arg::help, arg_type::flag, "Print the help screen to stdout"},
      {"-q", "--quiet", "", arg::quiet, arg_type::flag, "Quiet mode, only the final result will be printed to stdout."},
      {"-d", "--dry-run", "", arg::dry_run, arg_type::flag, "Dry run, execute cucumber without invoking steps. Steps will still be checked if they are defined"},
      {"-v", "--verbose", "", arg::verbose, arg_type::flag, "Print detailed information, e.g. skipped scenarios, tag evaluation"},
      {"-c", "--continue-on-failure", "", arg::continue_on_failure, arg_type::flag, "Do not skip subsequent steps in a scenario after a failed step, all steps will run regardless of intermediate failures\n"},

      {"-t", "--tags", "<EXPRESSION>", arg::tags, arg_type::option, tag_description},
      {"-n", "--name", "<PATTERN>", arg::name_filter, arg_type::option, "Run only scenarios whose names match the given pattern. Supports '*' (zero or more characters) and '?' (exactly one character). Multiple patterns can be separated by ':'"},

      {"", "--report-json", "<opt: FILE>", arg::report_json, arg_type::option, "Print the test results as json to stdout or a given file"},
      {"", "--steps-catalog", "<opt: FILE>", arg::steps_catalog_readable, arg_type::option, "Write the implemented steps as readable text to stdout or a file"},
      {"", "--steps-catalog-json", "<opt: FILE>", arg::steps_catalog_json, arg_type::option, "Write the implemented steps as json text to stdout or a file\n"},

      {"", "--exclude-file", "", arg::exclude_file, arg_type::exclude_file, "Exclude a specific feature file from the test run\n"},
  };
  // clang-format on

 public:
  void initialize(int argc, const char* argv[]);
  void clear();

  bool is_set(program_args::arg key) const;
  const std::string& get_value(program_args::arg key) const;
  const std::vector<feature_file>& get_feature_files() const;
  const std::vector<std::string>& get_excluded_files() const;

 private:
  void process_path(std::string_view sv);
  void find_feature_in_dir(const std::filesystem::path& dir);
  void remove_excluded_files();

 private:
  std::unordered_map<arg, std::pair<bool, std::string>>
      m_options;                          // e.g. --tags "expression"
  std::unordered_map<arg, bool> m_flags;  // e.g. --quiet
  std::vector<feature_file> m_files;
  std::vector<std::string> m_excluded_files;

  static const std::unordered_map<std::string_view, info> keys;
};

[[nodiscard]] program_args& get_program_args(
    std::optional<int> argc = std::nullopt, const char* argv[] = nullptr);

static void print_help_screen()
{
  log::info(std::format("CWT-Cucumber {}: A C++ Cucumber Interpreter",
                        cuke::version::as_string()));
  constexpr const char* helptest = R"(
  Usage:
    ./<your-executable> ./<file>.feature [options]
    ./<your-executable> ./<dir> [options]
  Executing single scenarios from line (multiple lines possible): 
   <your-executable> ./<file>.feature:4 [options]
   <your-executable> ./<file>.feature:4:10:15 [options]

  Options:)";
  log::info(helptest, log::new_line);
  for (const auto& def : get_program_args().defs)
  {
    log::info("    ", def.short_key.empty() ? "  " : def.short_key, " ",
              def.long_key, def.value.empty() ? "" : " ", def.value, "  ",
              def.description, log::new_line);
  }
}

namespace internal
{

class runtime_options
{
 public:
  struct manual_fail
  {
    bool is_set;
    std::string msg;
  };

  runtime_options()
  {
    if (const char* env_p = std::getenv("CWT_CUCUMBER_STEP_DELAY"))
    {
      m_delay_ms = std::stoi(env_p);
    }
  }
  [[nodiscard]] bool skip_scenario() const noexcept { return m_skip_scenario; }
  void skip_scenario(bool value) noexcept { m_skip_scenario = value; }

  [[nodiscard]] bool ignore_scenario() const noexcept
  {
    return m_ignore_scenario;
  }
  void ignore_scenario(bool value) noexcept { m_ignore_scenario = value; }

  [[nodiscard]] manual_fail fail_scenario() const noexcept
  {
    return m_fail_scenario;
  }
  void reset_fail_scenario() noexcept
  {
    fail_scenario(false, m_default_scenario_failed_msg);
  }
  void fail_scenario(bool value, std::string_view msg = "") noexcept
  {
    m_fail_scenario.is_set = value;
    if (!msg.empty())
    {
      m_fail_scenario.msg = msg;
    }
  }

  [[nodiscard]] manual_fail fail_step() const noexcept { return m_fail_step; }
  void reset_fail_step() noexcept
  {
    fail_step(false, m_default_step_failed_msg);
  }
  void fail_step(bool value, std::string_view msg = "") noexcept
  {
    m_fail_step.is_set = value;
    if (!msg.empty())
    {
      m_fail_step.msg = msg;
    }
  }

  void sleep_if_has_delay()
  {
    if (m_delay_ms != 0)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(m_delay_ms));
    }
  }

 private:
  static constexpr const std::string_view m_default_scenario_failed_msg =
      "Scenario set to failed with 'cuke::fail_scenario()'";
  static constexpr const std::string_view m_default_step_failed_msg =
      "Step set to failed with 'cuke::fail_step()'";
  bool m_skip_scenario{false};
  bool m_ignore_scenario{false};
  manual_fail m_fail_scenario{
      manual_fail{false, std::string{m_default_scenario_failed_msg}}};
  manual_fail m_fail_step{
      manual_fail{false, std::string{m_default_step_failed_msg}}};
  long m_delay_ms{0};
};

runtime_options& get_runtime_options();

}  // namespace internal

}  // namespace cuke
