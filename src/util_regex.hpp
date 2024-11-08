#pragma once
#include <regex>
#include <unordered_set>

#include "param_info.hpp"
#include "expression.hpp"
#include "registry.hpp"

namespace cuke::internal
{

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
  std::regex pattern(cuke::registry().create_expression_key_regex_pattern());
  std::smatch match;
  std::vector<param_info> type_info;

  std::size_t offset = 0;

  while (std::regex_search(result, match, pattern))
  {
    const std::string key = match[0].str();
    const auto& conversion = cuke::registry().get_expression(key);
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
