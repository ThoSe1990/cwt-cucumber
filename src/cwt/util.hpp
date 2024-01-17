#pragma once

#include <string>
#include <iostream>

#include "value.hpp"

namespace cwt::details
{
enum class op_code : uint32_t
{
  constant,
  tag,
  nil,
  pop,
  get_var,
  set_var,
  define_var,
  print,
  println,
  step_result,
  init_scenario,
  scenario_result,
  jump_if_failed,
  call,
  call_step,
  call_step_with_doc_string,
  hook,
  func_return
};

enum class color
{
  standard,
  green,
  yellow,
  red,
  blue,
  black
};

class console_log
{
 public:
  console_log(color c, const std::string& str)
  {
    switch (c)
    {
      case color::standard:
      {
        std::cout << "\x1b[0m";
      }
      break;
      case color::green:
      {
        std::cout << "\x1b[32m";
      }
      break;
      case color::yellow:
      {
        std::cout << "\x1b[33m";
      }
      break;
      case color::red:
      {
        std::cout << "\x1b[31m";
      }
      break;
      case color::blue:
      {
        std::cout << "\x1b[34m";
      }
      break;
      case color::black:
      {
        std::cout << "\x1b[30m";
      }
    }
    std::cout << str;
  }
  ~console_log() { std::cout << "\x1b[0m"; }
};

static void print(color c, const std::string& str) { console_log(c, str); }
static void println(color c, const std::string& str)
{
  print(c, str);
  std::cout << '\n';
}

template <typename T>
inline constexpr uint32_t to_uint(T value)
{
  return static_cast<uint32_t>(value);
}
inline constexpr op_code to_code(uint32_t val)
{
  if (val >= to_uint(op_code::constant) && val <= to_uint(op_code::func_return))
      [[likely]]
  {
    return static_cast<op_code>(val);
  }
  else [[unlikely]]
  {
    throw std::out_of_range(
        "inline op_code to_code(uint32_t val): value out of range");
  }
}
inline constexpr hook_type to_hook_type(uint32_t val)
{
  if (val >= to_uint(hook_type::reset_context) &&
      val <= to_uint(hook_type::after_step)) [[likely]]
  {
    return static_cast<hook_type>(val);
  }
  else [[unlikely]]
  {
    throw std::out_of_range(
        "inline hook_type to_hook_type(uint32_t val): value out of range");
  }
}
inline constexpr color to_color(uint32_t val)
{
  if (val >= to_uint(color::standard) && val <= to_uint(color::black))
      [[likely]]
  {
    return static_cast<color>(val);
  }
  else [[unlikely]]
  {
    throw std::out_of_range(
        "inline color to_color(uint32_t val): value out of range");
  }
}

}  // namespace cwt::details
