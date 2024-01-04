#pragma once

#include <stack>
#include <string_view>

#include "value.hpp"
#include "chunk.hpp"

namespace cwt::details
{

struct call_frame
{
  chunk* chunk_ptr;
};

class vm
{
 public:
  void interpret(std::string_view source);
 private:
  void run();
  void call(const function& func);
 private:
  std::stack<value> m_stack;
  std::vector<call_frame> m_frames;
};

}  // namespace cwt::details
