#pragma once

#include <span>
#include <vector>
#include <string>


// TODO Remove
#include <iostream>

namespace cwt::details
{

struct options
{
  bool quiet{false};
  std::string tag_expression{""};
};

inline options create_options(int argc, char* argv[])
{
  std::span<char*> arguments(argv, argc);

  // Print out the arguments
  std::cout << "Number of arguments: " << arguments.size() << std::endl;
  for (const auto& arg : arguments)
  {
    std::cout << arg << std::endl;
  }
}

}  // namespace cwt::details
