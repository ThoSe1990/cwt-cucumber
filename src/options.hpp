#pragma once

#include <fstream>
#include <chrono>
#include <thread>
#include <span>
#include <vector>
#include <string>
#include <filesystem>

#include "log.hpp"
#include "version.hpp"

namespace cuke
{

struct feature_file
{
  std::string path;
  std::vector<std::size_t> lines_to_run;
};

class sink
{
 public:
  void try_to_set_file_sink(std::span<const char*>::iterator it,
                            std::span<const char*>::iterator end)
  {
    if (it == end)
    {
      return;
    }

    namespace fs = std::filesystem;
    fs::path path = *it;
    if (fs::is_directory(path))
    {
      return;
    }
    if (path.string().find(".feature") != std::string::npos)
    {
      return;
    }
    m_filepath = path.string();
  }

  void write(std::string_view data) const
  {
    if (m_filepath.empty())
    {
      log::info(data, log::new_line);
    }
    else
    {
      std::ofstream file(m_filepath);
      if (file.is_open())
      {
        file << data;
        file.close();
      }
      else
      {
        log::error(std::format("Can not open file '{}'", m_filepath));
      }
    }
  }

 private:
  std::string m_filepath;
};

enum class catalog_type
{
  none = 0,
  readable_text,
  json
};
enum class report_type
{
  none = 0,
  json
};

struct options
{
  bool print_help{false};
  bool continue_on_failure{false};
  bool dry_run{false};
  struct
  {
    catalog_type type{catalog_type::none};
    sink out;
  } catalog;
  struct
  {
    report_type type{report_type::none};
    sink out;
  } report;
  std::string tag_expression;
  std::vector<feature_file> files;
  std::vector<std::string> excluded_files;
};  // namespace cuke

class cuke_args
{
 public:
  void initialize(int argc, const char* argv[]);
  void clear();
  const options& get_options() const noexcept;

 private:
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

void skip_scenario();
void ignore_scenario();
void fail_scenario(const std::string_view msg = "");
void fail_step(const std::string_view msg = "");

}  // namespace cuke
