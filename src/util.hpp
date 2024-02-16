#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>

namespace cwt::details
{

enum class return_code
{
  passed,
  failed,
  undefined,
  skipped,
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
  print_linebreak,
  print_indent,
  print_step_result,
  init_scenario,
  scenario_result,
  jump_if_failed,
  call,
  call_step,
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

}  // namespace cwt::details

#include "token.hpp"
#include "value.hpp"

namespace cwt::details
{

template <typename T>
[[nodiscard]] inline constexpr uint32_t to_uint(T value)
{
  return static_cast<uint32_t>(value);
}
[[nodiscard]] inline constexpr op_code to_code(uint32_t val)
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
[[nodiscard]] inline constexpr color to_color(uint32_t val)
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

[[nodiscard]] inline color to_color(return_code result)
{
  switch (result)
  {
    case return_code::passed:
      return color::green;
    case return_code::failed:
      return color::red;
    case return_code::skipped:
      return color::blue;
    case return_code::undefined:
      return color::yellow;
    default:
      return color::standard;
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
static void print(std::string_view msg) { print(color::standard, msg); }

static void println(color c, std::string_view mgs)
{
  print(c, mgs);
  std::cout << '\n';
}
static void println() { std::cout << '\n'; }
static void println(std::string_view msg) { println(color::standard, msg); }

static constexpr std::string replace(const token& t, std::string_view r)
{
  std::string str{t.value};
  size_t pos;
  while ((pos = str.find(r)) != std::string::npos)
  {
    str.replace(pos, r.size(), "");
  }
  return str;
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
    case token_type::doc_string:
    {
      auto rm_3_at_start_and_end = [](const token& t){
        return value(std::string(t.value.substr(3, t.value.size()-6)));
      };
      return rm_3_at_start_and_end(t);
    }
    break;
    case token_type::string_value:
    {
      return value(replace(t, "\""));
    }
    break;
    default:
      println(color::red, std::format("util::token_to_value: Given token '{}' "
                                      "is invalid to create a value",
                                      t.value));
      return value{nil_value{}};
  }
}

[[nodiscard]] inline const value& to_value(argv values, std::size_t idx)
{
  return *(values + idx);
}

[[nodiscard]] inline value_array combine(const value_array& v1,
                                         const value_array& v2)
{
  value_array result;
  for (const auto& v : v1)
  {
    result.push_back(v);
  }
  for (const auto& v : v2)
  {
    result.push_back(v);
  }
  return result;
}

[[nodiscard]] inline return_code to_return_code(
    const std::unordered_map<return_code, std::size_t>& results)
{
  if (results.at(return_code::failed) == 0 &&
      results.at(return_code::skipped) == 0 &&
      results.at(return_code::undefined) == 0)
  {
    return return_code::passed;
  }
  else
  {
    return return_code::failed;
  }
}

[[nodiscard]] inline std::string to_string(return_code rc)
{
  switch (rc)
  {
    case return_code::passed:
      return std::string("passed");
    case return_code::failed:
      return std::string("failed");
    case return_code::skipped:
      return std::string("skipped");
    case return_code::undefined:
      return std::string("undefined");
    default:
      return std::string("");
  }
}

[[nodiscard]] inline std::size_t count_values(
    const std::unordered_map<return_code, std::size_t>& m)
{
  std::size_t count = 0;
  for (const auto& pair : m)
  {
    count += pair.second;
  }
  return count;
}

inline void print(const std::unordered_map<return_code, std::size_t>& results)
{
  bool need_comma = false;
  print(" (");
  for (const return_code& rc : {return_code::passed, return_code::failed,
                                return_code::skipped, return_code::undefined})
  {
    if (results.at(rc) > 0)
    {
      if (need_comma)
      {
        print(", ");
      }
      print(to_color(rc), std::format("{} {}", results.at(rc), to_string(rc)));
      need_comma = true;
    }
  }
  println(")");
}

[[nodiscard]] inline auto count_results(
    const std::vector<return_code>& scenario)
{
  return std::make_tuple(
      std::count(scenario.begin(), scenario.end(), return_code::passed),
      std::count(scenario.begin(), scenario.end(), return_code::failed),
      std::count(scenario.begin(), scenario.end(), return_code::skipped),
      std::count(scenario.begin(), scenario.end(), return_code::undefined));
}

[[nodiscard]] inline return_code scenario_result(std::size_t /*passed*/,
                                                 std::size_t failed,
                                                 std::size_t skipped,
                                                 std::size_t undefined)
{
  if (failed > 0)
  {
    return return_code::failed;
  }
  else if (skipped > 0)
  {
    return return_code::skipped;
  }
  else if (undefined > 0)
  {
    return return_code::undefined;
  }
  else
  {
    return return_code::passed;
  }
}

[[nodiscard]] inline std::unordered_map<return_code, std::size_t>
get_scenario_results(const std::vector<std::vector<return_code>>& scenarios)
{
  std::unordered_map<return_code, std::size_t> result{
      {return_code::passed, 0},
      {return_code::failed, 0},
      {return_code::skipped, 0},
      {return_code::undefined, 0}};
  for (const std::vector<return_code>& s : scenarios)
  {
    auto [passed, failed, skipped, undefined] = count_results(s);
    return_code current = scenario_result(passed, failed, skipped, undefined);
    result[current]++;
  }
  return result;
}
[[nodiscard]] inline std::unordered_map<return_code, std::size_t>
get_step_results(const std::vector<std::vector<return_code>>& scenarios)
{
  std::unordered_map<return_code, std::size_t> result{
      {return_code::passed, 0},
      {return_code::failed, 0},
      {return_code::skipped, 0},
      {return_code::undefined, 0}};

  for (const std::vector<return_code>& s : scenarios)
  {
    auto [passed, failed, skipped, undefined] = count_results(s);
    result[return_code::passed] += passed;
    result[return_code::failed] += failed;
    result[return_code::skipped] += skipped;
    result[return_code::undefined] += undefined;
  }
  return result;
}

[[nodiscard]] inline std::string step_prefix(return_code rc)
{
  switch (rc)
  {
    case return_code::passed:
      return std::string("[   PASSED    ]");
    case return_code::failed:
      return std::string("[   FAILED    ]");
    case return_code::skipped:
      return std::string("[   SKIPPED   ]");
    case return_code::undefined:
      return std::string("[  UNDEFINED  ]");
    default:
      return std::string("");
  }
}

[[nodiscard]] inline bool is_number(std::string_view sv)
{
  for (const char& c : sv)
  {
    if (c < '0' || c > '9')
    {
      return false;
    }
  }
  return true;
}

[[nodiscard]] inline std::pair<std::string, std::vector<unsigned long>>
filepath_and_lines(std::string_view sv)
{
  std::vector<unsigned long> lines;
  long pos = static_cast<long>(sv.size());
  long last_pos = pos;
  while (pos > 0)
  {
    pos = static_cast<long>(sv.rfind(':', pos));
    const std::string_view sub = sv.substr(pos + 1, last_pos - pos);
    if (is_number(sub))
    {
      lines.push_back(std::stoul(sub.data()));
    }
    else
    {
      break;
    }
    --pos;
    last_pos = pos;
  }
  return std::make_pair(std::string{sv.substr(0, last_pos + 1)}, lines);
}

[[nodiscard]] inline std::string read_file(const std::string& path)
{
  std::ifstream in(path);
  std::string script((std::istreambuf_iterator<char>(in)),
                     std::istreambuf_iterator<char>());
  return script;
}

}  // namespace cwt::details
