#pragma once

#include <chrono>
#include <string>
#include <fstream>

#include "token.hpp"

namespace cuke::internal
{
[[nodiscard]] static std::string to_string_with_linebreaks(
    const std::vector<std::string>& data)
{
  std::ostringstream oss;
  for (size_t i = 0; i < data.size(); ++i)
  {
    oss << data[i];
    if (i + 1 < data.size()) oss << '\n';
  }
  return oss.str();
}
[[nodiscard]] static std::string to_string(const std::vector<std::string>& data)
{
  std::ostringstream oss;
  for (const auto& str : data)
  {
    oss << str << ' ';
  }
  std::string result = oss.str();
  if (!result.empty())
  {
    result.pop_back();
  }
  return result;
}

[[nodiscard]] inline bool quoted_string(const std::string& str)
{
  return (str[0] == '"' && str[str.size() - 1] == '"');
}

[[nodiscard]] inline std::string remove_quotes(const std::string& str)
{
  return (str[0] == '"' && str[str.size() - 1] == '"')
             ? str.substr(1, str.size() - 2)
             : str;
}

[[nodiscard]] inline std::string create_string(std::string_view sv,
                                               std::size_t drop_chars = 0)
{
  return std::string(sv.substr(0, sv.size() - drop_chars));
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

[[nodiscard]] inline std::pair<std::string, std::vector<std::size_t>>
filepath_and_lines(std::string_view sv)
{
  std::vector<std::size_t> lines;
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

static std::string_view rtrim(std::string_view str)
{
  std::size_t end = str.size();
  while (end > 0 && std::isspace(static_cast<unsigned char>(str[end - 1])))
  {
    --end;
  }
  return str.substr(0, end);
}

}  // namespace cuke::internal
