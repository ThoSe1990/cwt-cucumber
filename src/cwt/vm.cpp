#include <iostream>
#include "vm.hpp"

namespace cwt::details
{

void vm::run(const chunk& c) 
{
  for (auto it = c.cbegin() ; it != c.cend() ; ++it)
  {
    std::cout << static_cast<unsigned int>(*it) << std::endl;
  }
}

}  // namespace cwt::details