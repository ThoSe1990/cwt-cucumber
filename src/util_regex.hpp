

#include "token.hpp"

#include <array>
#include <vector>
#include <unordered_set>

namespace cuke::internal
{

struct regex_conversion
{
  std::string_view key;
  std::string_view pattern;
  token_type type;
};

static constexpr const std::array<regex_conversion, 9> conversions = {{
    {"{byte}", "(-?\\d+|<[^>]+>)", token_type::parameter_byte},
    {"{int}", "(-?\\d+|<[^>]+>)", token_type::parameter_int},
    {"{short}", "(-?\\d+|<[^>]+>)", token_type::parameter_int},
    {"{long}", "(-?\\d+|<[^>]+>)", token_type::parameter_long},
    {"{float}", "(-?\\d*\\.?\\d+|<[^>]+>)", token_type::parameter_float},
    {"{double}", "(-?\\d*\\.?\\d+|<[^>]+>)", token_type::parameter_double},
    {"{word}", "([^\\s<]+|<[^>]+>)", token_type::parameter_word},
    {"{string}", "(\"[^\"]*\"|<[^>]+>)", token_type::parameter_string},
    {"{}", "(.+|<[^>]+>)", token_type::parameter_anonymous},
}};

static constexpr const regex_conversion& get_regex_conversion(
    std::string_view key)
{
  auto it = std::find_if(conversions.begin(), conversions.end(),
                         [&key](const regex_conversion& conversion)
                         { return conversion.key == key; });

  if (it != conversions.end())
  {
    return (*it);
  }

  throw std::runtime_error("Conversion not found");
}

static constexpr const std::pair<std::string, std::vector<token_type>>
create_regex_definition(std::string_view input)
{
  std::string result;
  result.reserve(input.size());

  std::vector<token_type> types;

  size_t i = 0;
  while (i < input.size())
  {
    if (input[i] == '{')
    {
      size_t end = input.find('}', i);
      if (end != std::string::npos)
      {
        std::string placeholder(input.substr(i, end - i + 1));
        const auto& conversion = get_regex_conversion(placeholder);
        result += conversion.pattern;
        types.push_back(conversion.type);
        i = end + 1;
        continue;
      }
    }
    result += input[i];
    ++i;
  }
  result = "^" + result + "$";
  return {result, types};
}

static const std::unordered_set<char> special_chars = {
    '.', '^', '$', '*', '+', '?', '[', ']', '(', ')', '\\', '|'};

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
