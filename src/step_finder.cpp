#include <regex>
#include <algorithm>
#include <string_view>

#include "step_finder.hpp"

#include "token.hpp"
#include "value.hpp"
#include "util.hpp"

namespace cuke::internal
{

namespace
{

struct regex_conversion
{
  std::string_view key;
  std::string_view pattern;
  token_type type;
};

constexpr const static std::array<regex_conversion, 9> conversions = {{
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

constexpr static const regex_conversion& get_regex_conversion(
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

std::pair<std::string, std::vector<token_type>> create_regex_definition(
    std::string_view input)
{
  std::string result;
  result.reserve(input.size());  // Reserve space for efficiency

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

  return {result, types};
}

}  // namespace

step_finder::step_finder(std::string_view feature) : m_feature_string(feature)
{
}
step_finder::step_finder(std::string_view feature, cuke::table::row hash_row)
    : m_feature_string(feature), m_hash_row(std::move(hash_row))
{
}
cuke::value_array& step_finder::values() noexcept { return m_values; }
bool step_finder::step_matches(std::string_view defined_step)
{
  bool found = false;
  {
    auto [pattern, types] = create_regex_definition(defined_step);
    pattern = "^" + pattern + "$";
    std::regex regex_pattern(pattern);
    std::smatch match;
    std::string text = m_feature_string;
    while (std::regex_search(text, match, regex_pattern))
    {
      found = true;
      for (std::size_t i = 1; i < match.size(); ++i)
      {
        if (!match[i].matched)
        {
          continue;
        }
        const std::string& value = match[i];
        if (match[i].str()[0] == '<')
        {
          m_values.push_back(m_hash_row[value.substr(1, value.size() - 2)]);
          continue;
        }

        switch (types[i - 1])
        {
          case token_type::parameter_byte:
          case token_type::parameter_short:
          case token_type::parameter_int:
          case token_type::parameter_long:
          {
            long long_value = std::stol(value);
            m_values.push_back(cuke::value(long_value));
          }
          break;
          case token_type::parameter_float:
          {
            float float_value = std::stof(value);
            m_values.push_back(cuke::value(float_value));
          }
          break;
          case token_type::parameter_double:
          {
            double double_value = std::stod(value);
            m_values.push_back(cuke::value(double_value));
          }
          break;
          case token_type::parameter_anonymous:
          case token_type::parameter_word:
            m_values.push_back(cuke::value(value));
            break;
          case token_type::parameter_string:
            m_values.push_back(cuke::value(value.substr(1, value.size() - 2)));
            break;
          default:
            println(
                color::red,
                std::format("step_finder: Can not convert {} to a cuke::value",
                            value));
        }
      }
      text = match.suffix().str();
    }
  }
  return found;
}

}  // namespace cuke::internal
