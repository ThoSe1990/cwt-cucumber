#include <iostream>
#include "vm.hpp"

namespace cwt::details
{

void vm::run(const chunk& c) 
{
  for (auto it = c.begin() ; it != c.end() ; ++it)
  {
    std::cout << static_cast<unsigned int>(*it) << std::endl;
  }
}

}  // namespace cwt::details