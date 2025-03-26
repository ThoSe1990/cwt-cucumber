#include "step.hpp"

#include "util_regex.hpp"

#include <iostream>
namespace cuke::internal
{

step::step(step_callback cb, const std::string& definition,
           type step_type /* = type::step */,
           const std::string& function_name /* = "" */,
           const std::string& file /* = "" */, std::size_t line /*  = 0 */)
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
const std::string& step::function_name() const noexcept
{
  return m_function_name;
}
const std::string& step::definition() const noexcept { return m_definition; }
const std::vector<param_info>& step::type_info() const noexcept
{
  return m_type_info;
}
const std::string& step::regex_string() const noexcept
{
  return m_regex_definition;
}
void step::call(const value_array& values,
                const std::vector<std::string>& doc_string,
                const table& t) const
{
  m_callback(values, m_type_info, doc_string, t);
}
step::type step::step_type() const noexcept { return m_type; }

std::string step::source_location() const noexcept
{
  return std::format("{}:{}", m_file, m_line);
}
// TODO: is this dead?
std::string to_string(step::type type)
{
  switch (type)
  {
    case step::type::given:
      return "Given";
    case step::type::when:
      return "When";
    case step::type::then:
      return "Then";
    case step::type::step:
      return "Step";
    default:
      return "...";
  }
}
}  // namespace cuke::internal
