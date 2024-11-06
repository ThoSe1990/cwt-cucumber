#pragma once
#include <vector>
#include <string>

#include "value.hpp"

namespace cuke::internal
{

using step_callback = void (*)(const cuke::value_array& args,
                               const std::vector<param_info>& info,
                               const std::vector<std::string>& doc_string,
                               const cuke::table& t);

class step
{
 public:
  enum class type
  {
    given,
    when,
    then,
    step
  };

  step(step_callback cb, const std::string& definition,
       type step_type = type::step, const std::string& function_name = "");
  const std::string& function_name() const noexcept;
  const std::string& definition() const noexcept;
  const std::vector<param_info>& type_info() const noexcept;
  const std::string& regex_string() const noexcept;
  void call(const value_array& values,
            const std::vector<std::string>& doc_string, const table& t) const;
  type step_type() const noexcept;

 private:
  step_callback m_callback;
  std::string m_definition;
  std::string m_regex_definition;
  std::vector<param_info> m_type_info;
  std::string m_function_name;
  type m_type;
};

[[nodiscard]] std::string to_string(step::type type);

}  // namespace cuke::internal
