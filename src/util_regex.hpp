#pragma once
#include <array>
#include <regex>
#include <format>
#include <algorithm>
#include <stdexcept>
#include <unordered_set>

#include "param_info.hpp"
#include "expression.hpp"
#include "registry.hpp"

namespace cuke::internal
{
// TODO: move this to the cuke::registry
static /* constexpr */ const std::array<const expression, 10>
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
        {"{pair of integers}", R"(var1=(\d+), var2=(\d+))", "two integers"}
        //  "var1 = (\\d+), var2 = (\\d+)", "two integers"}
    }};

static /* constexpr */ const expression& get_regex_conversion(
    std::string_view key)
{
  auto it = std::find_if(default_conversions.begin(), default_conversions.end(),
                         [&key](const expression& conversion)
                         { return conversion.key == key; });

  if (it != default_conversions.end()) [[likely]]
  {
    return (*it);
  }
  throw std::runtime_error(
      std::format("Regex conversion for key '{}' not found!", key));
  // return cuke::registry().get_custom_conversion(key);
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

    type_info.push_back(
        {offset, value_count, conversion.key, conversion.type_info});

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
