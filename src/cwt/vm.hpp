#pragma once

#include <deque>
#include <string_view>
#include <unordered_map>

#include "value.hpp"
#include "chunk.hpp"

namespace cwt::details
{

struct call_frame
{
  chunk* chunk_ptr;
  chunk::const_iterator it;
};

class vm
{
 public:
  void interpret(std::string_view source);

 private:
  void run();
  void call(const function& func);

 private:
  std::deque<value> m_stack;
  std::vector<call_frame> m_frames;
  std::unordered_map<std::string, value> m_globals;
};

}  // namespace cwt::details
