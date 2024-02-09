#pragma once 

#include <string>

namespace cwt::details
{
  
struct options
{
  bool quiet{false};
  std::string tag_expression{""};
};


} // namespace cwt::details
