#pragma once

#include <stack>
#include <string_view>

#include "value.hpp"

namespace cwt::details
{

class vm
{
 public:
  void run(std::string_view script);

 private:
 private:
  std::stack<value> m_stack;
};

}  // namespace cwt::details
