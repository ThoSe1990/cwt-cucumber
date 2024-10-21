#include <regex>
#include <string_view>

#include "step_finder.hpp"

#include "token.hpp"
#include "value.hpp"
#include "util.hpp"

namespace cuke::internal
{

step_finder::step_finder(std::string_view feature) : m_feature_string(feature)
{
}
step_finder::step_finder(std::string_view feature, cuke::table::row hash_row)
    : m_feature_string(feature), m_hash_row(std::move(hash_row))
{
}
cuke::value_array& step_finder::values() noexcept { return m_values; }
bool step_finder::step_matches(const std::string& pattern,
                               const std::vector<token_type>& types)
{
  bool found = false;
  {
    // auto [pattern, types] = create_regex_definition(defined_step);
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
          push_value(m_hash_row[value.substr(1, value.size() - 2)].to_string(),
                     types[i - 1]);
        }
        else
        {
          push_value(value, types[i - 1]);
        }
      }
      text = match.suffix().str();
    }
  }
  return found;
}

void step_finder::push_value(const std::string& value, token_type type)
{
  switch (type)
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
    {
      if (value[0] == '"' && value[value.size() - 1] == '"')
      {
        m_values.push_back(cuke::value(value.substr(1, value.size() - 2)));
      }
      else
      {
        m_values.push_back(cuke::value(value));
      }
    }
    break;
    default:
      println(color::red,
              std::format("step_finder: Can not convert {} to a cuke::value",
                          value));
  }
}
}  // namespace cuke::internal
