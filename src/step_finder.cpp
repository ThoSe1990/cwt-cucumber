#include <regex>
#include <format>
#include <string_view>

#include "step_finder.hpp"
#include "token.hpp"
#include "util.hpp"
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
  string
};
std::pair<std::string, std::vector<value_type>> create_regex_definition(
    std::string_view input)
{
  std::string result;
  result.reserve(input.size());  // Reserve space for efficiency

  const std::unordered_map<std::string, std::pair<std::string, value_type>>
      conversions = {
          {"{byte}", {"(-?\\d+)", value_type::byte}},
          {"{int}", {"(-?\\d+)", value_type::int_}},
          {"{short}", {"(-?\\d+)", value_type::short_}},
          {"{long}", {"(-?\\d+)", value_type::long_}},
          {"{float}", {"(-?\\d*\\.?\\d+)", value_type::float_}},
          {"{double}", {"(-?\\d*\\.?\\d+)", value_type::double_}},
          {"{word}", {"([^\\s]+)", value_type::word}},
          {"{string}", {"\"([^\"]*)\"", value_type::string}},
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

std::string create_regex_definition_angle_brackets(std::string_view input)
{
  std::string result;
  result.reserve(input.size());  // Reserve space for efficiency

  size_t i = 0;
  while (i < input.size())
  {
    if (input[i] == '<')
    {
      // Check for closing angle bracket
      size_t end = input.find('>', i);
      if (end != std::string::npos)
      {
        // Add the regex pattern to capture any non-whitespace characters
        result += "([^\\s>]+)";  // Regex for anything between angle brackets
        i = end + 1;             // Move past the closing bracket
        continue;                // Skip the default processing
      }
    }
    // If not a placeholder, just copy the character
    result += input[i];
    ++i;
  }
  return result;
}
static void correct_floating_point_types(token& defined, token& feature)
{
  if (feature.type == token_type::double_value &&
      defined.type == token_type::parameter_float)
  {
    feature.type = token_type::float_value;
  }
}
step_finder::step_finder(std::string_view feature) : m_feature_string(feature)
{
}
step_finder::step_finder(std::string_view feature, cuke::table::row hash_row)
    : m_feature(feature), m_hash_row(std::move(std::move(hash_row)))
{
}
cuke::value_array& step_finder::values() noexcept { return m_values; }
bool step_finder::step_matches(std::string_view defined_step)
{
  bool found = false;
  {
    auto [pattern, types] = create_regex_definition(defined_step);

    std::regex regex_pattern(pattern);
    std::smatch match;
    std::string text = m_feature_string;

    while (std::regex_search(text, match, regex_pattern))
    {
      found = true;
      for (size_t i = 1; i < match.size(); ++i)
      {
        const std::string& value = match[i];
        std::cout << "type: " << int(types[i - 1]) << " value: " << value
                  << std::endl;
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
          case value_type::word:
          case value_type::string:
            m_values.push_back(cuke::value(value));
            break;
        }
      }
      text = match.suffix().str();
    }
  }
  return found;
  if (found)
  {
    return true;
  }

  {
    auto pattern = create_regex_definition_angle_brackets(defined_step);

    std::regex regex_pattern(pattern);
    std::smatch match;
    std::string text = m_feature_string;

    while (std::regex_search(text, match, regex_pattern))
    {
      found = true;
      for (size_t i = 1; i < match.size(); ++i)
      {
        const std::string& value = match[i];
        std::cout << " value: " << value << std::endl;
      }
    }
  }

  return found;
}
// bool step_finder::step_matches(std::string_view defined_step)
// {
//   m_feature.reset();
//   m_values.clear();
//   m_defined = scanner(defined_step);
//   for (;;)
//   {
//     // auto [defined, feature] = next();
//     auto defined = m_defined.scan_token();
//     auto feature = next_feature_token(defined);
//
//     // auto delimiter = m_defined.scan_token();
//     // feature = m_feature.scan_token_until(delimiter);
//     // defined = delimiter;
//     // std::cout << defined.value << "  == " << feature.value << std::endl;
//
//     while (defined.type == token_type::parameter_word)
//     {
//       std::cout << "defined is " << defined.value << std::endl;
//       defined = m_defined.scan_token();
//       std::cout << "after first is " << defined.value << std::endl;
//
//       defined = m_defined.scan_token();
//       std::cout << "after second is " << defined.value << std::endl;
//
//       feature = m_feature.scan_token_until(defined);
//       std::cout << defined.value << "  =++= " << feature.value <<
//       std::endl;
//       m_values.push_back(cuke::value(create_string(feature.value)));
//       // feature = next_feature_token(defined);
//     }
//
//     if (is_parameter(defined.type))
//     {
//       bool negative = false;
//       if (feature.type == token_type::minus)
//       {
//         negative = true;
//         feature = m_feature.scan_token();
//       }
//       else if (parameter_matches_value(defined.type, feature.type))
//       {
//         correct_floating_point_types(defined, feature);
//         m_values.push_back(token_to_value(feature, negative));
//       }
//       else if (feature.type == token_type::variable)
//       {
//         m_values.push_back(
//             m_hash_row[feature.value.substr(1, feature.value.size() - 2)]);
//       }
//       else
//       {
//         return false;
//       }
//     }
//     else if (is_not_equal(defined, feature))
//     {
//       return false;
//     }
//
//     if (is_at_end(defined) && is_at_end(feature))
//     {
//       return true;
//     }
//   }
// }

cuke::value step_finder::create_table()
{
  cuke::value_array values;
  std::size_t cols_count = 0;

  for (;;)
  {
    token current = m_feature.scan_token();
    cols_count = 0;
    while (current.type != token_type::linebreak && !is_at_end(current))
    {
      std::vector<token> tokens;
      while (current.type != token_type::vertical)
      {
        tokens.push_back(current);
        current = m_feature.scan_token();
      }
      values.push_back(tokens_to_value(tokens));
      current = m_feature.scan_token();
      ++cols_count;
    }

    while (current.type == token_type::linebreak)
    {
      current = m_feature.scan_token();
    }

    if (is_at_end(current))
    {
      return cuke::value(make_table_ptr(std::move(values), cols_count));
    }
  }
}

bool step_finder::parameter_matches_value(token_type parameter, token_type type)
{
  switch (parameter)
  {
    case token_type::parameter_int:
      return type == token_type::long_value;
    case token_type::parameter_word:
      return type == token_type::word;
    case token_type::parameter_string:
      return type == token_type::string_value;
    case token_type::parameter_byte:
      return type == token_type::long_value;
    case token_type::parameter_short:
      return type == token_type::long_value;
    case token_type::parameter_long:
      return type == token_type::long_value;
    case token_type::parameter_float:
    case token_type::parameter_double:
      return type == token_type::float_value ||
             type == token_type::double_value;
    default:
    {
      println(color::red, std::format("step_finder: Invalid token_type: '{}'",
                                      to_uint(parameter)));
      return false;
    }
  }
}
bool step_finder::is_parameter(token_type parameter)
{
  return parameter >= token_type::parameter_int &&
         parameter <= token_type::parameter_long;
}
bool step_finder::is_at_end(const token& t)
{
  return t.type == token_type::eof;
}

bool step_finder::is_not_equal(const token& lhs, const token& rhs)
{
  return lhs.value != rhs.value;
}

cuke::value step_finder::make_word_parameter(const token begin)
{
  token defined = m_defined.scan_token();
  token feature = m_feature.scan_token();
  token last = feature;
  while (defined.value != feature.value)
  {
    last = feature;
    feature = m_feature.scan_token();
  }
  return cuke::value(create_string(begin, last));
}
std::pair<token, token> step_finder::next()
{
  return {m_defined.scan_token(), m_feature.scan_token()};
}

token step_finder::next_feature_token(const token& defined)
{
  if (defined.type == token_type::word)
  {
    return m_feature.scan_word(defined.value.length());
  }
  return m_feature.scan_token();
}

}  // namespace cuke::internal
