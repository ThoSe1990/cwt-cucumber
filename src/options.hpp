#pragma once

#include <fstream>
#include <memory>
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
      println(internal::color::red,
              std::format("Sink: Can't set directory '{}' as output sink",
                          path.string()));
      return;
    }
    if (path.string().find(".feature") != std::string::npos)
    {
      println(internal::color::red,
              std::format("Sink: Can't set feature file '{}' as output sink",
                          path.string()));
      return;
    }
    m_file_stream = std::make_unique<std::ofstream>(path.string());
    if (m_file_stream->is_open())
    {
      m_sink = m_file_stream.get();
    }
    else
    {
      println(internal::color::red,
              std::format("Sink: Failed to open file '{}'", path.string()));
    }
  }

  void write(std::string_view data) const
  {
    if (m_sink)
    {
      *m_sink << data << '\n';
    }
  }

 private:
  std::ostream* m_sink{&std::cout};
  std::unique_ptr<std::ofstream> m_file_stream;
};

struct options
{
  bool quiet{false};
  bool print_help{false};
  struct
  {
    bool readable_text{false};
    bool json{false};
    sink out;
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
  void process_option(std::span<const char*>::iterator it,
                      std::span<const char*>::iterator end);
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

  println("Executing single scenarios from line (multiple lines possible): ");
  println("  <your-executable> ./<file>.feature:4");
  println("  <your-executable> ./<file>.feature:4:10:15\n");

  println("Options:");
  println("  -h --help\t\t\t\tPrint the help screen to stdout");
  println(
      "  -q --quiet\t\t\t\tQuiet mode, only the final result will be printed "
      "to stdout.");
  println(
      "  --step-catalog [opt: file]\t\tWrite the implemented steps as "
      "readable text to stdout or a file");
  println(
      "\t\t\t\t\tthis overwrites existing files, does not write to "
      "'.feature' files or directories");
  println(
      "  --step-catalog-json [opt: file]\tWrite the implemented steps as "
      "readable text to stdout or a file");
  println(
      "\t\t\t\t\tthis overwrites existing files, does not write to "
      "'.feature' files or directories");

  println(
      "  -t --tags \"expression\"\t\tProvide a tag expression to execute only "
      "Features/Scenarios with given tags");
  println("\t\t\t\t\tExamples:");
  println("\t\t\t\t\t  \"@tag1\"");
  println("\t\t\t\t\t  \"@tag1 or @tag2\"");
  println("\t\t\t\t\t  \"not @tag2\"");
  println("\t\t\t\t\t  \"(@tag1 and @tag2) or not @tag3\"");
  println("\t\t\t\t\t  \"((@tag1 and @tag2) or @tag3) xor @tag4\"");
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
