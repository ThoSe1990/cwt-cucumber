#pragma once 

#include <vector> 
#include <string> 

namespace cwt::details
{

struct file
{
  std::string path;
  std::string content;
  std::vector<unsigned long> lines_to_compile;
};
using feature_files = std::vector<file>;

} // namespace cwt::details