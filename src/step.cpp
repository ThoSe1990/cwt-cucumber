#include "step.hpp"

#include "util_regex.hpp"

#include <iostream>
namespace cuke::internal
{

step_definition::step_definition(step_callback cb,
                                 const std::string& definition,
                                 type step_type /* = type::step */,
                                 const std::string& function_name /* = "" */,
                                 const std::string& file /* = "" */,
                                 std::size_t line /*  = 0 */)
    : m_callback(cb),
      m_definition(definition),
      m_type(step_type),
      m_function_name(function_name),
      m_file(file),
      m_line(line)
{
  std::tie(m_regex_definition, m_type_info) =
      create_regex_definition(add_escape_chars(m_definition));
}
const std::string& step_definition::function_name() const noexcept
{
  return m_function_name;
}
const std::string& step_definition::definition() const noexcept
{
  return m_definition;
}
const std::vector<param_info>& step_definition::type_info() const noexcept
{
  return m_type_info;
}
const std::string& step_definition::regex_string() const noexcept
{
  return m_regex_definition;
}
void step_definition::call(const value_array& values,
                           const std::vector<std::string>& doc_string,
                           const table& t) const
{
  m_callback(values, m_type_info, doc_string, t);
}
step_definition::type step_definition::step_type() const noexcept
{
  return m_type;
}

std::string step_definition::source_location() const noexcept
{
  return std::format("{}:{}", m_file, m_line);
}

std::string to_string(step_definition::type type)
{
  switch (type)
  {
    case step_definition::type::given:
      return "Given";
    case step_definition::type::when:
      return "When";
    case step_definition::type::then:
      return "Then";
    case step_definition::type::step:
      return "Step";
    default:
      return "...";
  }
}
}  // namespace cuke::internal
