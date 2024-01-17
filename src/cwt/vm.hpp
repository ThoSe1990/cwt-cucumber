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

  void push_step(const step& s);

  void push_before(const hook& h);
  void push_after(const hook& h);
  void push_before_step(const hook& h);
  void push_after_step(const hook& h);

 private:
  void run();
  void call(const function& func);

 private:
  std::deque<value> m_stack;
  std::vector<call_frame> m_frames;
  std::unordered_map<std::string, value> m_globals;

  std::vector<step> m_steps;

  std::vector<hook> m_before;
  std::vector<hook> m_after;
  std::vector<hook> m_before_step;
  std::vector<hook> m_after_step;
};

}  // namespace cwt::details
