#include <format>

#include "step_finder.hpp"

namespace cwt::details
{

step_finder::step_finder(std::string_view implemented, std::string_view feature)
    : m_implemented(implemented), m_feature(feature)
{
}

bool step_finder::step_matches()
{
  for (;;)
  {
    auto [implemented, feature] = next();
    if (is_parameter(implemented.type))
    {
      bool negative = false;
      if (feature.type == token_type::minus)
      {
        negative = true;
        feature = m_feature.scan_token();
      }
      if (parameter_matches_value(implemented.type, feature.type))
      {
        make_value(feature, negative);
        // m_values.push_back(make_value(feature, negative));
      }
      else
      {
        return false;
      }
    }
    else if (is_not_equal(implemented, feature))
    {
      return false;
    }
    else if (is_at_end(implemented) && is_at_end(feature))
    {
      return true;
    }
  }
}

void step_finder::make_value(const token& t, bool negative)
{
  switch (t.type)
  {
    case token_type::long_value:
    {
      long v = std::stol(t.value.data());
      if (negative)
      {
        v *= -1;
      }
      m_values.push_back(value{v});
    }
    break;
    case token_type::double_value:
    {
      double v = std::stod(t.value.data());
      if (negative)
      {
        v *= -1;
      }
      m_values.push_back(value{v});
    }
    break;
    case token_type::string_value:
    {
      m_values.push_back(value{std::string(t.value)});
    }
    break;
    default:
      println(color::red,
              std::format("step_finder: Given token '{}' is invalid", t.value));
  }
}
bool step_finder::parameter_matches_value(token_type parameter,
                                          token_type value)
{
  switch (parameter)
  {
    case token_type::parameter_int:
      return value == token_type::long_value;
    case token_type::parameter_float:
      return value == token_type::double_value;
    case token_type::parameter_word:
      return value == token_type::string_value;
    case token_type::parameter_string:
      return value == token_type::string_value;
    case token_type::parameter_double:
      return value == token_type::double_value;
    case token_type::parameter_byte:
      return value == token_type::long_value;
    case token_type::parameter_short:
      return value == token_type::long_value;
    case token_type::parameter_long:
      return value == token_type::long_value;
    default: 
    {
      println(color::red,
              std::format("step_finder: Invaklid token_type: '{}'", to_uint(parameter)));
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

std::pair<token, token> step_finder::next()
{
  return {m_implemented.scan_token(), m_feature.scan_token()};
}

}  // namespace cwt::details
