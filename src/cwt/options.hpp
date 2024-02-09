#pragma once

#include <span>
#include <vector>
#include <string>
#include <filesystem>

#include "util.hpp"

// TODO Remove
#include <iostream>

namespace cwt::details
{

struct feature_file
{
  std::string path;
  std::vector<unsigned long> lines;
};

struct options
{
  bool quiet{false};
  std::vector<feature_file> files;
  std::string tag_expression{""};
};

static options terminal_options;

inline void create_options(int argc, const char* argv[])
{
  std::span<const char*> arguments(argv, argc);

  // Print out the arguments
  std::cout << "Number of arguments: " << arguments.size() << std::endl;
  for (const auto& arg : arguments)
  {
    std::string_view sv{arg};
    if (sv.starts_with('-'))
    {
      // TODO 
    }
    else
    {
      auto [file, lines] = filepath_and_lines(sv);
      namespace fs = std::filesystem;
      fs::path path = file;
      if (fs::exists(path)) 
      {
        if (fs::is_directory(path)) 
        {
          std::cout << "TODO: recursive file search" << std::endl;
          // find_feature_in_dir(path);
        } 
        else if (path.extension() == ".feature") 
        {
          terminal_options.files.push_back(feature_file{file, lines});
        }
      }
    }
  }
}

}  // namespace cwt::details
