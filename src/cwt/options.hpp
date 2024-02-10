#pragma once

#include <span>
#include <vector>
#include <string>
#include <filesystem>

#include "compiler/tags.hpp"

namespace cwt::details
{

struct file
{
  std::string path;
  std::vector<unsigned long> lines;
};
using feature_files = std::vector<file>;

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

}  // namespace cwt::details
