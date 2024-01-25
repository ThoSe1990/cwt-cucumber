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
  [[nodiscard]] return_code interpret(std::string_view source);

  static void push_step(const step& s);
  static void push_hook_before(const hook& h);
  static void push_hook_after(const hook& h);
  static void push_hook_before_step(const hook& h);
  static void push_hook_after_step(const hook& h);

 private:
  [[nodiscard]] static std::vector<step>& steps();
  [[nodiscard]] static std::vector<hook>& before();
  [[nodiscard]] static std::vector<hook>& after();
  [[nodiscard]] static std::vector<hook>& before_step();
  [[nodiscard]] static std::vector<hook>& after_step();

  void runtime_error(std::string_view msg);

  void run();
  void call(const function& func);

 private:
  std::deque<value> m_stack;
  std::vector<call_frame> m_frames;
  std::unordered_map<std::string, value> m_globals;
};

}  // namespace cwt::details
