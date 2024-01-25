#pragma once

#include <string>
#include <iostream>
#include <unordered_map>

#include "value.hpp"

namespace cwt::details
{

enum class return_code
{
  passed,
  failed,
  compile_error,
  runtime_error
};

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

static const std::unordered_map<color, std::string> color_codes = {
    {color::standard, "\x1b[0m"}, {color::green, "\x1b[32m"},
    {color::yellow, "\x1b[33m"},  {color::red, "\x1b[31m"},
    {color::blue, "\x1b[34m"},    {color::black, "\x1b[30m"}};

static void print(color c, std::string_view msg)
{
  auto it = color_codes.find(c);
  if (it != color_codes.end())
  {
    std::cout << it->second;
  }
  else
  {
    std::cerr << "Color code " << to_uint(c) << " not found!\n";
  }
  std::cout << msg << "\x1b[0m";
}

static void println(color c, const std::string& str)
{
  print(c, str);
  std::cout << '\n';
}

static bool step_matches(std::string_view implemented, std::string_view feature)
{
  return false;
}

}  // namespace cwt::details
