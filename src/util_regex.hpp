#pragma once
#include <array>
#include <regex>
#include <format>
#include <algorithm>
#include <unordered_set>

#include "param_info.hpp"
#include "regex_conversion.hpp"

namespace cuke::internal
{

static /* constexpr */ const std::array<regex_conversion, 9>
    default_conversions = {{
        {"{byte}", "(-?\\d+)", "byte"},
        {"{int}", "(-?\\d+)", "int"},
        {"{short}", "(-?\\d+)", "short"},
        {"{long}", "(-?\\d+)", "long"},
        {"{float}", "(-?\\d*\\.?\\d+)", "float"},
        {"{double}", "(-?\\d*\\.?\\d+)", "double"},
        {"{word}", "([^\\s<]+)", "word"},
        {"{string}", "\"(.*?)\"", "string"},
        {"{}", "(.+)", "anonymous"},
    }};

static /* constexpr */ const regex_conversion& get_regex_conversion(
    std::string_view key)
{
  {
    auto it =
        std::find_if(default_conversions.begin(), default_conversions.end(),
                     [&key](const regex_conversion& conversion)
                     { return conversion.key == key; });

    if (it != default_conversions.end()) [[likely]]
    {
      return (*it);
    }
  }
  {
    auto it = std::find_if(custom_conversions.begin(), custom_conversions.end(),
                           [&key](const regex_conversion& conversion)
                           { return conversion.key == key; });

    if (it != custom_conversions.end()) [[likely]]
    {
      return (*it);
    }
  }
  throw std::runtime_error(std::format("No conversion to '{}' found.", key));
}

[[nodiscard]] static std::string create_word_alternation(
    const std::string& step)
{
  std::string result = step;

  result = std::regex_replace(result, std::regex("\\)"), ")?");
  result = std::regex_replace(result, std::regex("\\("), "(?:");

  std::regex pattern("(\\w+)/(\\w+)");
  std::smatch match;

  while (std::regex_search(result, match, pattern))
  {
    result = std::regex_replace(
        result, pattern, "(?:" + match[1].str() + "|" + match[2].str() + ")",
        std::regex_constants::format_first_only);
  }

  return result;
}

[[nodiscard]] static /* constexpr */ const std::pair<std::string,
                                                     std::vector<param_info>>
create_regex_definition(const std::string& step)
{
  std::string test = step;
  std::string result = '^' + create_word_alternation(step);
  std::regex pattern("\\{(.*?)\\}");
  std::smatch match;

  std::vector<param_info> type_info;

  std::size_t offset = 0;

  while (std::regex_search(result, match, pattern))
  {
    const auto& conversion = get_regex_conversion(match[0].str());
    result = std::regex_replace(result, pattern, conversion.pattern,
                                std::regex_constants::format_first_only);
    const std::size_t value_count = std::regex(conversion.pattern).mark_count();
    const std::size_t zero_based_v_count = value_count - 1;

    type_info.push_back({offset, value_count, conversion.type_info});

    if (value_count > 0)
    {
      offset += zero_based_v_count;
    }
  }

  result += '$';
  return {result, type_info};
}

static const std::unordered_set<char> special_chars = {
    '.', '^', '$', '*', '+', '?', '[', ']', /* '(', ')', */ '\\',
    /* '|' */};

static std::string add_escape_chars(const std::string& input)
{
  std::string result;
  for (char c : input)
  {
    if (special_chars.find(c) != special_chars.end())
    {
      result += '\\';
    }
    result += c;
  }
  return result;
}

}  // namespace cuke::internal
