#pragma once

#include <string>
#include <iostream>
#include <unordered_map>

#include "token.hpp"
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
  hook_before,
  hook_before_step,
  hook_after,
  hook_after_step,
  reset_context,
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

[[nodiscard]] static value token_to_value(const token& t, bool negative)
{
  switch (t.type)
  {
    case token_type::long_value:
    {
      long v = std::stol(t.value.data());
      if (negative)
      {
        v *= -1;
      }
      return value(v);
    }
    break;
    case token_type::double_value:
    {
      double v = std::stod(t.value.data());
      if (negative)
      {
        v *= -1;
      }
      return value(v);
    }
    break;
    case token_type::string_value:
    {
      return value(std::string(t.value));
    }
    break;
    default:
      println(
          color::red,
          std::format(
              "util::token_to_value: Given token '{}' is invalid to create a value",
              t.value));
      return value{nil_value{}};
  }
}

inline const value& to_value(argv values, std::size_t idx)
{
  return *(values + idx);
}

}  // namespace cwt::details
