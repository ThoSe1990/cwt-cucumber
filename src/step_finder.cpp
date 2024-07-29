#include <format>

#include "step_finder.hpp"
#include "util.hpp"

namespace cuke::internal
{

static void correct_floating_point_types(token& defined,
                                                       token& feature)
{
  if (feature.type == token_type::double_value &&
      defined.type == token_type::parameter_float)
  {
    feature.type = token_type::float_value;
  }
}
step_finder::step_finder(std::string_view defined, std::string_view feature)
    : m_defined(defined), m_feature(feature)
{
}
step_finder::step_finder(std::string_view feature) : m_feature(feature) {}
std::size_t step_finder::values_count() const noexcept
{
  return m_values.size();
}

cuke::value& step_finder::get_value(std::size_t idx)
{
  if (idx < m_values.size())
  {
    return m_values[idx];
  }
  else
  {
    throw std::out_of_range("step_finder::get_value: Index out of range");
  }
}

void step_finder::reset_with_next_step(std::string_view defined) noexcept
{
  m_feature.reset();
  m_values.clear();
  m_defined = scanner(defined);
}
bool step_finder::step_matches()
{
  for (;;)
  {
    auto [defined, feature] = next();
    if (is_parameter(defined.type))
    {
      bool negative = false;
      if (feature.type == token_type::minus)
      {
        negative = true;
        feature = m_feature.scan_token();
      }
      if (parameter_matches_value(defined.type, feature.type))
      {
        correct_floating_point_types(defined, feature);
        m_values.push_back(token_to_value(feature, negative));
      }
      else if (feature.type == token_type::variable)
      {
        auto use_nil_as_placeholder = [this]()
        { m_values.push_back(nil_value{}); };
        use_nil_as_placeholder();
      }
      else
      {
        return false;
      }
    }
    else if (is_at_end(defined) && feature.type == token_type::linebreak)
    {
      while (feature.type == token_type::linebreak)
      {
        feature = m_feature.scan_token();
      }

      if (feature.type == token_type::doc_string)
      {
        m_values.push_back(token_to_value(feature, false));
      }
      else if (feature.type == token_type::vertical)
      {
        cuke::value v = create_table();
        m_values.push_back(v);
      }
      feature = m_feature.scan_token();
    }
    else if (is_not_equal(defined, feature))
    {
      return false;
    }

    if (is_at_end(defined) && is_at_end(feature))
    {
      return true;
    }
  }
}

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
      return type == token_type::string_value;
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

std::pair<token, token> step_finder::next()
{
  return {m_defined.scan_token(), m_feature.scan_token()};
}

}  // namespace cuke::internal
