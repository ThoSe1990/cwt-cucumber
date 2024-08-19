#pragma once

#include <chrono>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>

#include "test_results.hpp"

namespace cuke::internal
{

enum class color
{
  standard,
  green,
  yellow,
  red,
  blue,
  black
};

}  // namespace cuke::internal

#include "token.hpp"
#include "value.hpp"

namespace cuke::internal
{

[[nodiscard]] inline std::string create_string(std::string_view sv)
{
  return std::string(sv);
}
[[nodiscard]] inline std::string create_string(std::string_view begin,
                                               std::string_view end)
{
  return std::string(begin.data(), end.data() + end.size());
}
[[nodiscard]] inline std::string create_string(const token& begin,
                                               const token& end)
{
  return create_string(begin.value, end.value);
}

[[nodiscard]] inline std::pair<std::string, std::string>
split_on_first_linebreak(const std::string& str)
{
  std::size_t pos = str.find('\n');
  if (pos != std::string::npos)
  {
    return {str.substr(0, pos), str.substr(pos + 1)};
  }
  else
  {
    return {str, ""};
  }
}

template <typename T>
[[nodiscard]] inline constexpr uint32_t to_uint(T value)
{
  return static_cast<uint32_t>(value);
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

[[nodiscard]] inline color to_color(cuke::results::test_status status)
{
  switch (status)
  {
    case cuke::results::test_status::passed:
      return color::green;
    case cuke::results::test_status::failed:
      return color::red;
    case cuke::results::test_status::skipped:
      return color::blue;
    case cuke::results::test_status::undefined:
      return color::yellow;
    default:
      return color::standard;
  }
}

static const std::unordered_map<color, std::string> color_codes = {
    {color::standard, "\x1b[0m"}, {color::green, "\x1b[32m"},
    {color::yellow, "\x1b[33m"},  {color::red, "\x1b[31m"},
    {color::blue, "\x1b[34m"},    {color::black, "\x1b[90m"}};

template <typename T>
inline void print_impl(const T& t)
{
  std::cout << t;
}

template <typename... Args>
inline void print(color c, Args&&... args)
{
  auto it = color_codes.find(c);
  if (it != color_codes.end())
  {
    std::cout << it->second;
    (print_impl(std::forward<Args>(args)), ...);
    std::cout << color_codes.at(color::standard);
  }
  else
  {
    std::cerr << "Color code " << to_uint(c) << " not found!\n";
  }
}
template <typename... Args>
inline void print(Args&&... args)
{
  (print_impl(std::forward<Args>(args)), ...);
}
template <typename... Args>
inline void println(Args&&... args)
{
  (print_impl(std::forward<Args>(args)), ...);
  std::cout << '\n';
}
template <typename... Args>
inline void println(color c, Args&&... args)
{
  print(c, std::forward<Args>(args)...);
  std::cout << '\n';
}
inline void println() { std::cout << '\n'; }
inline void println(std::string_view msg) { println(color::standard, msg); }

inline constexpr std::string replace(const token& t, std::string_view r)
{
  std::string str{t.value};
  size_t pos;
  while ((pos = str.find(r)) != std::string::npos)
  {
    str.replace(pos, r.size(), "");
  }
  return str;
}

[[nodiscard]] inline cuke::value token_to_value(const token& t, bool negative)
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
      return cuke::value(v);
    }
    break;
    case token_type::double_value:
    {
      double v = std::stod(t.value.data());
      if (negative)
      {
        v *= -1;
      }
      return cuke::value(v);
    }
    break;
    case token_type::float_value:
    {
      float v = std::stof(t.value.data());
      if (negative)
      {
        v *= -1;
      }
      return cuke::value(v);
    }
    break;
    case token_type::doc_string:
    {
      auto rm_3_at_start_and_end = [](const token& t) {
        return cuke::value(std::string(t.value.substr(3, t.value.size() - 6)));
      };
      return rm_3_at_start_and_end(t);
    }
    break;
    case token_type::string_value:
    case token_type::word:
    {
      return cuke::value(replace(t, "\""));
    }
    break;
    default:
      println(color::red, std::format("util::token_to_value: Given token '{}' "
                                      "is invalid to create a value",
                                      t.value));
      return cuke::value{nil_value{}};
  }
}
[[nodiscard]] inline cuke::value tokens_to_value(
    const std::vector<token>& tokens)
{
  if (tokens.size() == 1)
  {
    return token_to_value(tokens[0], false);
  }
  else if (tokens.size() == 2 && tokens[0].type == token_type::minus &&
           (tokens[1].type == token_type::long_value ||
            tokens[1].type == token_type::double_value))
  {
    return token_to_value(tokens[1], true);
  }
  else if (tokens.size() == 0)
  {
    println(color::red, "Unexpected nil value");
    return cuke::value(nil_value{});
  }
  else
  {
    return cuke::value(create_string(tokens[0].value, tokens.back().value));
  }
}

[[nodiscard]] inline cuke::value_array combine(const cuke::value_array& v1,
                                               const cuke::value_array& v2)
{
  cuke::value_array result;
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

[[nodiscard]] inline std::string to_string(cuke::results::test_status status)
{
  switch (status)
  {
    case cuke::results::test_status::passed:
      return std::string("passed");
    case cuke::results::test_status::failed:
      return std::string("failed");
    case cuke::results::test_status::skipped:
      return std::string("skipped");
    case cuke::results::test_status::undefined:
      return std::string("undefined");
    default:
      return std::string("");
  }
}

[[nodiscard]] inline std::string step_prefix(cuke::results::test_status status)
{
  switch (status)
  {
    case cuke::results::test_status::passed:
      return std::string("[   PASSED    ] ");
    case cuke::results::test_status::failed:
      return std::string("[   FAILED    ] ");
    case cuke::results::test_status::skipped:
      return std::string("[   SKIPPED   ] ");
    case cuke::results::test_status::undefined:
      return std::string("[   UNDEFINED ] ");
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

[[nodiscard]] inline std::string read_file(std::string_view path)
{
  std::ifstream in(path.data());
  std::string script((std::istreambuf_iterator<char>(in)),
                     std::istreambuf_iterator<char>());
  return script;
}

[[nodiscard]] inline table_ptr make_table_ptr(cuke::value_array values,
                                              std::size_t cols_count)
{
  return std::make_unique<cuke::table>(std::move(values), cols_count);
}

template <typename T>
struct result_and_time
{
  T return_value;
  double execution_time;
};

template <typename Result, typename Func, typename... Args,
          typename = std::enable_if_t<!std::is_same_v<void, Result>>>
[[nodiscard]] result_and_time<Result> execute_and_count_time(Func&& func,
                                                             Args&&... args)
{
  auto start = std::chrono::high_resolution_clock::now();
  Result result = func(std::forward<Args>(args)...);
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration = end - start;
  return result_and_time<Result>{result, duration.count()};
}

template <typename Func, typename... Args>
[[nodiscard]] double execute_and_count_time(Func&& func, Args&&... args)
{
  auto start = std::chrono::high_resolution_clock::now();
  func(std::forward<Args>(args)...);
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration = end - start;
  return duration.count();
}

}  // namespace cuke::internal
