#pragma once

#include <stack>

#include "chunk.hpp"

namespace cwt::details
{

class vm
{
 public:
  void run(const chunk& c);
 private:
 private:
  std::stack<value> m_stack;
  
};

}  // namespace cwt::details
