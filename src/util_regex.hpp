#include "token.hpp"

#include <iostream>

#include <array>
#include <regex>
#include <algorithm>
#include <unordered_set>

namespace cuke::internal
{

struct regex_conversion
{
  std::string key;
  std::string pattern;
  token_type type;
};

static /* constexpr */ const std::array<regex_conversion, 9> conversions = {{
    {"{byte}", "(-?\\d+)", token_type::parameter_byte},
    {"{int}", "(-?\\d+)", token_type::parameter_int},
    {"{short}", "(-?\\d+)", token_type::parameter_int},
    {"{long}", "(-?\\d+)", token_type::parameter_long},
    {"{float}", "(-?\\d*\\.?\\d+)", token_type::parameter_float},
    {"{double}", "(-?\\d*\\.?\\d+)", token_type::parameter_double},
    {"{word}", "([^\\s<]+)", token_type::parameter_word},
    {"{string}", "(\"[^\"]*\"|\".*?)", token_type::parameter_string},
    {"{}", "(.+)", token_type::parameter_anonymous},
}};

static /* constexpr */ const regex_conversion& get_regex_conversion(
    std::string_view key)
{
  auto it = std::find_if(conversions.begin(), conversions.end(),
                         [&key](const regex_conversion& conversion)
                         { return conversion.key == key; });

  if (it != conversions.end()) [[likely]]
  {
    return (*it);
  }

  throw std::runtime_error("Conversion not found");
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
        result, pattern, "(" + match[1].str() + "|" + match[2].str() + ")",
        std::regex_constants::format_first_only);
  }

  return result;
}

[[nodiscard]] static /* constexpr */ const std::pair<std::string,
                                                     std::vector<token_type>>
create_regex_definition(const std::string& step)
{
  std::string test = step;
  std::string result = '^' + create_word_alternation(step);
  std::regex pattern("\\{(.*?)\\}");
  std::smatch match;

  std::vector<token_type> types;

  while (std::regex_search(result, match, pattern))
  {
    const auto& conversion = get_regex_conversion(match[0].str());
    result = std::regex_replace(result, pattern, conversion.pattern,
                                std::regex_constants::format_first_only);
    types.push_back(conversion.type);
  }

  result += '$';
  return {result, types};
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
