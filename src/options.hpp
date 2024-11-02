#pragma once

#include <span>
#include <vector>
#include <string>
#include <filesystem>

#include "util.hpp"
#include "version.hpp"

namespace cuke
{

struct feature_file
{
  std::string path;
  std::vector<std::size_t> lines_to_run;
};

struct options
{
  bool quiet{false};
  bool print_help{false};
  struct
  {
    bool readable_text{false};
    bool json{false};
  } catalog;
  std::string tag_expression;
  std::vector<feature_file> files;
};

class cuke_args
{
 public:
  void initialize(int argc, const char* argv[]);
  void clear();
  const options& get_options() const noexcept;

 private:
  void process_path(std::string_view sv);
  void process_option(std::span<const char*>::iterator it);
  void find_feature_in_dir(const std::filesystem::path& dir);

 private:
  std::span<const char*> m_args;
  options m_options;
};

[[nodiscard]] cuke_args& program_arguments(
    std::optional<int> argc = std::nullopt, const char* argv[] = nullptr);

static void print_help_screen()
{
  println(std::format("CWT-Cucumber {}: A C++ Cucumber Interpreter",
                                cuke::version::as_string()));
  println("Usage:");
  println("  ./<your-executable> ./<file>.feature [options]");
  println("  ./<your-executable> ./<dir> [options]\n");

  println(
      "Executing single scenarios from line (multiple lines possible): ");
  println("  <your-executable> ./<file>.feature:4");
  println("  <your-executable> ./<file>.feature:4:10:15\n");

  println("Options:");
  println("  -h --help\t\t\tPrint the help screen to stdout");
  println(
      "  -q --quiet\t\t\tQuiet mode, only the final result will be printed to "
      "stdout.");
  println(
      "  -t --tags \"expression\"\tProvide a tag expression to execute only "
      "Features/Scenarios with given tags");
  println("\t\t\t\tExamples:");
  println("\t\t\t\t  \"@tag1\"");
  println("\t\t\t\t  \"@tag1 or @tag2\"");
  println("\t\t\t\t  \"not @tag2\"");
  println("\t\t\t\t  \"(@tag1 and @tag2) or not @tag3\"");
  println("\t\t\t\t  \"((@tag1 and @tag2) or @tag3) xor @tag4\"");
}

namespace internal
{

class runtime_options
{
 public:
  [[nodiscard]] bool skip_scenario() const noexcept { return m_skip_scenario; }
  void skip_scenario(bool value) noexcept { m_skip_scenario = value; }

 private:
  bool m_skip_scenario{false};
};

runtime_options& get_runtime_options();

}  // namespace internal

void skip_scenario();

}  // namespace cuke
