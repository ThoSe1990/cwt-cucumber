#pragma once
#include <format>
#include <regex>
#include <unordered_set>

#include "param_info.hpp"
#include "expression.hpp"
#include "registry.hpp"
#include "table.hpp"
#include "log.hpp"

namespace cuke::internal
{

[[nodiscard]] static std::string replace_variables(
    const std::string& step, const table::row& row,
    bool ignore_missing_key = false)
{
  std::regex pattern("<(.*?)>");
  std::smatch match;
  std::string result;
  std::string::const_iterator search_start(step.cbegin());

  while (std::regex_search(search_start, step.cend(), match, pattern))
  {
    std::string key = match[1].str();
    if (!row.contains(key))
    {
      // No matching Examples column for this placeholder. This can be a
      // deliberate non-Examples angle-bracket tag (e.g. XML/HTML in a doc
      // string, ignore_missing_key == true) or a genuine authoring mistake
      // (typo'd placeholder, missing Examples column). Either way, never
      // throw on malformed/mismatched input - leave the original text
      // untouched and keep searching after it.
      if (ignore_missing_key)
      {
        cuke::log::verbose(
            std::format("[   VERBOSE   ] Doc string key '<{}>' not found in "
                        "Examples, left unchanged",
                        key),
            cuke::log::new_line);
      }
      else
      {
        cuke::log::error(
            std::format("Placeholder '<{}>' not found in Examples, left "
                        "unchanged",
                        key),
            cuke::log::new_line);
      }
      result.append(search_start, match[0].second);
      search_start = match[0].second;
    }
    else
    {
      result.append(search_start, match[0].first);
      const std::string& value = row[key].to_string();
      result += value.empty() ? "\"\"" : value;
      search_start = match[0].second;
    }
  }
  result.append(search_start, step.cend());

  return result;
}

[[nodiscard]] static std::string create_word_alternation(
    const std::string& step)
{
  std::string result;
  result.reserve(step.size());

  for (std::size_t i = 0; i < step.size(); ++i)
  {
    const char c = step[i];

    if (c == '\\' && i + 1 < step.size() &&
        (step[i + 1] == '(' || step[i + 1] == ')' || step[i + 1] == '{' ||
         step[i + 1] == '}'))
    {
      result += '\\';
      result += step[i + 1];
      ++i;
    }
    else if (c == '(')
    {
      result += "(?:";
    }
    else if (c == ')')
    {
      result += ")?";
    }
    else
    {
      result += c;
    }
  }

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

    type_info.push_back({offset, value_count, key, conversion.type_info});

    if (value_count > 0)
    {
      offset += zero_based_v_count;
    }
  }

  result += '$';
  return {result, type_info};
}

static std::string add_escape_chars(const std::string& input)
{
  static const std::unordered_set<char> special_chars = {
      '.', '^', '$', '*', '+', '?', '[', ']', /* '(', ')', */ '\\',
      /* '|' */};

  std::string result;
  for (std::size_t i = 0; i < input.size(); ++i)
  {
    const char c = input[i];

    if (c == '\\' && i + 1 < input.size() &&
        (input[i + 1] == '(' || input[i + 1] == ')' || input[i + 1] == '{' ||
         input[i + 1] == '}'))
    {
      result += c;
      result += input[i + 1];
      ++i;
      continue;
    }

    if (special_chars.find(c) != special_chars.end())
    {
      result += '\\';
    }
    result += c;
  }
  return result;
}

}  // namespace cuke::internal
