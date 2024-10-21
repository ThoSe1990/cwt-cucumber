#include <regex>
#include <string_view>

#include "step_finder.hpp"

#include "token.hpp"
#include "value.hpp"
#include "util.hpp"

namespace cuke::internal
{

std::pair<std::string, std::vector<token_type>> create_regex_definition(
    std::string_view input)
{
  std::string result;
  result.reserve(input.size());  // Reserve space for efficiency

  const std::unordered_map<std::string, std::pair<std::string, token_type>>
      conversions = {
          {"{byte}", {"(-?\\d+|<[^>]+>)", token_type::parameter_byte}},
          {"{int}", {"(-?\\d+|<[^>]+>)", token_type::parameter_int}},
          {"{short}", {"(-?\\d+|<[^>]+>)", token_type::parameter_int}},
          {"{long}", {"(-?\\d+|<[^>]+>)", token_type::parameter_long}},
          {"{float}",
           {"(-?\\d*\\.?\\d+|<[^>]+>)", token_type::parameter_float}},
          {"{double}",
           {"(-?\\d*\\.?\\d+|<[^>]+>)", token_type::parameter_double}},
          {"{word}", {"([^\\s<]+|<[^>]+>)", token_type::parameter_word}},
          {"{string}", {"(\"[^\"]*\"|<[^>]+>)", token_type::parameter_string}},
          {"{}", {"(.+|<[^>]+>)", token_type::parameter_anonymous}},
      };

  std::vector<token_type> types;

  size_t i = 0;
  while (i < input.size())
  {
    if (input[i] == '{')
    {
      // Check if we have a placeholder
      size_t end = input.find('}', i);
      if (end != std::string::npos)
      {
        std::string placeholder(input.substr(i, end - i + 1));
        auto it = conversions.find(placeholder);
        if (it != conversions.end())
        {
          // Replace with regex pattern
          result += it->second.first;  // Add the regex pattern to the result
          types.push_back(
              it->second.second);  // Add the corresponding token_type
          i = end + 1;             // Move past the closing bracket
          continue;                // Skip the default processing
        }
      }
    }
    // If not a placeholder, just copy the character
    result += input[i];
    ++i;
  }

  return {result, types};
}

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
