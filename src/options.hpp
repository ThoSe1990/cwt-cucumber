#pragma once

#include <span>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>

#include "util.hpp"
#include "file.hpp"
#include "version.hpp"
#include "compiler/tags.hpp"

namespace cwt::details
{

struct options
{
  bool quiet{false};
  compiler::tag_expression tags;
};

class terminal_arguments
{
 public:
  terminal_arguments(int argc, const char* argv[]);
  const options& get_options() const noexcept;
  const feature_files& get_files() const noexcept;

 private:
  void process();
  void process_path(std::string_view sv);
  void process_option(std::span<const char*>::iterator it);
  void find_feature_in_dir(const std::filesystem::path& dir);

 private:
  std::span<const char*> m_args;
  options m_options;
  feature_files m_feature_files;
};

static void do_print_help();
[[nodiscard]] static bool print_help(int argc, const char* argv[])
{
  std::span<const char*> terminal_args(argv, argc);

  if (terminal_args.size() == 1)
  {
    do_print_help();
    return true;
  }
  else if (terminal_args.size() == 2)
  {
    std::string_view arg = terminal_args[1];
    if (arg == std::string_view("-h") || arg == std::string_view("--help"))
    {
      do_print_help();
      return true;
    }
  }
  return false;
}

static void do_print_help()
{
  println(std::format("CWT-Cucumber {}: A C++ Cucumber Interpreter",
                      cuke::version::as_string()));
  println("Usage:");
  println("  ./<your-executable> ./<file>.feature [options]");
  println("  ./<your-executable> ./<dir> [options]\n");

  println("Executing single scenarios from line (multiple lines possible): ");
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

}  // namespace cwt::details
