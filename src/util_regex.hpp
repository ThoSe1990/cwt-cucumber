#pragma once
#include <cctype>
#include <format>
#include <optional>
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

  // A literal "{...}" group whose contents look like a regex repetition
  // quantifier (only digits and at most one comma, e.g. "{56}", "{2,}",
  // "{1,3}") is otherwise passed straight through unescaped by the loop
  // below - {int}/{word}/{your_type} etc. must stay untouched so they are
  // still recognized as expression keys later in create_regex_definition().
  // Left as-is, std::regex interprets such a group as an actual quantifier
  // rather than literal text; when it follows a wildcard-producing
  // expression like {} (-> "(.*)"), the result (e.g. "(.*){56}") triggers
  // catastrophic backtracking in std::regex_match on sufficiently long
  // input - effectively an unbounded hang (found via fuzz testing). So we
  // detect this shape here and escape the braces to keep it literal,
  // unless the whole "{...}" token is itself a registered expression key
  // (standard or custom) - e.g. a user may register a numeric-looking
  // custom expression such as "{56}", which must keep matching normally.
  auto is_quantifier_shaped =
      [&input](std::size_t open) -> std::optional<std::size_t>
  {
    std::size_t i = open + 1;
    bool has_digit = false;
    bool has_comma = false;
    for (; i < input.size(); ++i)
    {
      const char c = input[i];
      if (c == '}')
      {
        return has_digit ? std::optional<std::size_t>(i) : std::nullopt;
      }
      if (std::isdigit(static_cast<unsigned char>(c)))
      {
        has_digit = true;
      }
      else if (c == ',' && !has_comma)
      {
        has_comma = true;
      }
      else
      {
        return std::nullopt;
      }
    }
    return std::nullopt;
  };

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

    if (c == '{')
    {
      if (const auto close = is_quantifier_shaped(i))
      {
        // A registered custom expression key may itself look like a
        // quantifier (e.g. a user registering "{56}" as a custom type).
        // Such keys must stay unescaped so they are still recognized by
        // create_regex_definition()'s expression lookup - only escape
        // when the whole "{...}" token is *not* a known expression key.
        const std::string token(input, i, *close - i + 1);
        if (!cuke::registry().has_expression(token))
        {
          result += "\\{";
          result.append(input, i + 1, *close - i - 1);
          result += "\\}";
          i = *close;
          continue;
        }
      }
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
