#pragma once

#include <cstdint>
#include <chrono>
#include <functional>
#include <optional>
#include <string_view>
#include <thread>
#include <span>
#include <unordered_map>
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
  std::vector<std::size_t> lines_to_run;
};

}  // namespace cuke

namespace cuke
{

struct options
{
  bool dry_run{false};
  std::vector<feature_file> files;
  std::vector<std::string> excluded_files;

  enum class key : int8_t
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
    excluded_files,
    tags,
  };
  std::unordered_map<key, std::pair<bool, std::string>>
      options;                          // e.g. --tags "expression"
  std::unordered_map<key, bool> flags;  // e.g. --quiet
  std::vector<std::string> positional;  // e.g files

  static const std::unordered_map<std::string, key> long_keys;
  static const std::unordered_map<std::string, key> short_keys;
  enum class type : int8_t
  {
    option = 0,
    flag,
    positional,
  };
  static const std::unordered_map<key, type> key_type;
  static const std::unordered_map<std::string, key> key_lookup;
};  // namespace cuke

class cuke_args
{
 public:
  void initialize(int argc, const char* argv[]);
  void clear();
  const options& get_options() const noexcept;

 private:
  std::pair<cuke::options::type, cuke::options::key> to_internal_key(
      const std::string& option) const;
  void process_path(std::string_view sv);
  void process_option(std::span<const char*>::iterator it,
                      std::span<const char*>::iterator end);
  void find_feature_in_dir(const std::filesystem::path& dir);
  void remove_excluded_files();

 private:
  std::span<const char*> m_args;
  options m_options;
};

[[nodiscard]] cuke_args& program_arguments(
    std::optional<int> argc = std::nullopt, const char* argv[] = nullptr);

[[nodiscard]] const std::vector<std::string> get_positional_args();
[[nodiscard]] const bool program_arg_is_set(options::key key);
[[nodiscard]] const std::string& get_program_option_value(options::key key);

static void print_help_screen()
{
  log::info(std::format("CWT-Cucumber {}: A C++ Cucumber Interpreter",
                        cuke::version::as_string()));
  constexpr const char* helptest = R"(
  Usage:
    ./<your-executable> ./<file>.feature [options]
    ./<your-executable> ./<dir> [options]
  Executing single scenarios from line (multiple lines possible): 
   <your-executable> ./<file>.feature:4"
   <your-executable> ./<file>.feature:4:10:15

  Options:
    -h --help  Print the help screen to stdout
    
    -q --quiet  Quiet mode, only the final result will be printed to stdout.

    -d --dry-run Dry run, execute cucumber without invoking steps. Steps will still be checked if they are defined

    -v --verbose  Print detailed information, e.g. skipped scenarios, tag evaluation

    -c --continue-on-failure  Do not skip subsequent steps in a scenario after a failed step, 
                              all steps will run regardless of intermediate failures

    --report-json [opt: file]  Print the test results as json to stdout or a given file

    --steps-catalog [opt: file]       Write the implemented steps as readable text to stdout or a file 
    --steps-catalog-json [opt: file]  Write the implemented steps as json text to stdout or a file 
      Note: Catalog option overwrites existing files, does not write to '.feature' files or directories
  
    --exclude-file "file"   Exclude a specific feature file from the test run

    -t --tags "expression"  Provide a tag expression to execute only Features/Scenarios with given tags
      Examples:
        "@tag1"
        "@tag1 or @tag2"
        "not @tag2"
        "(@tag1 and @tag2) or not @tag3"
        "((@tag1 and @tag2) or @tag3) xor @tag4"
)";
  log::info(helptest, log::new_line);
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
