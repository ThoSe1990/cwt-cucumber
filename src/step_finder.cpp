#include <regex>
#include <string_view>

#include "step_finder.hpp"
#include "token.hpp"
#include "value.hpp"

namespace cuke::internal
{

enum class value_type
{
  byte,
  short_,
  int_,
  long_,
  float_,
  double_,
  word,
  string,
  anonymous
};
std::pair<std::string, std::vector<value_type>> create_regex_definition(
    std::string_view input)
{
  std::string result;
  result.reserve(input.size());  // Reserve space for efficiency

  const std::unordered_map<std::string, std::pair<std::string, value_type>>
      conversions = {
          {"{byte}", {"(-?\\d+|<[^>]+>)", value_type::byte}},
          {"{int}", {"(-?\\d+|<[^>]+>)", value_type::int_}},
          {"{short}", {"(-?\\d+|<[^>]+>)", value_type::short_}},
          {"{long}", {"(-?\\d+|<[^>]+>)", value_type::long_}},
          {"{float}", {"(-?\\d*\\.?\\d+|<[^>]+>)", value_type::float_}},
          {"{double}", {"(-?\\d*\\.?\\d+|<[^>]+>)", value_type::double_}},
          {"{word}", {"([^\\s<]+|<[^>]+>)", value_type::word}},
          {"{string}", {"(\"[^\"]*\"|<[^>]+>)", value_type::string}},
          {"{}", {"(.+|<[^>]+>)", value_type::anonymous}},
      };

  std::vector<value_type> types;

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
              it->second.second);  // Add the corresponding value_type
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
          case value_type::byte:
          case value_type::short_:
          case value_type::int_:
          case value_type::long_:
          {
            long long_value = std::stol(value);
            m_values.push_back(cuke::value(long_value));
          }
          break;
          case value_type::float_:
          {
            float float_value = std::stof(value);
            m_values.push_back(cuke::value(float_value));
          }
          break;
          case value_type::double_:
          {
            double double_value = std::stod(value);
            m_values.push_back(cuke::value(double_value));
          }
          break;
          case value_type::anonymous:
          case value_type::word:
            m_values.push_back(cuke::value(value));
            break;
          case value_type::string:
            m_values.push_back(cuke::value(value.substr(1, value.size() - 2)));
            break;
        }
      }
      text = match.suffix().str();
    }
  }
  return found;
}

}  // namespace cuke::internal
