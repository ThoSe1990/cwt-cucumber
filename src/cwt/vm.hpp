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

using hook_function = void(*)();
struct hook 
{
  hook_function function;
  // TODO tags
};

class vm
{
 public:
  void interpret(std::string_view source);

  void push_step(cuke_step step, const std::string& name);

  void push_before(hook_function hook);
  void push_after(hook_function hook);
  void push_before_step(hook_function step);
  void push_after_step(hook_function step);

 private:
  void run();
  void call(const function& func);

 private:
  std::deque<value> m_stack;
  std::vector<call_frame> m_frames;
  std::unordered_map<std::string, value> m_globals;

  std::unordered_map<std::string, cuke_step> m_steps;

  using hooks = std::vector<void(*)()>;
  hooks m_before;
  hooks m_after;
  hooks m_before_step;
  hooks m_after_step;
};

}  // namespace cwt::details
