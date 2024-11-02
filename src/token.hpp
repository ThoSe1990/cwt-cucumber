#pragma once

#include <format>
#include <string>
#include <string_view>

namespace cuke::internal
{
enum class token_type
{
  vertical,
  minus,
  language,
  feature,
  background,
  scenario,
  scenario_outline,
  step,
  examples,
  variable,
  tag,
  _and,
  _or,
  _xor,
  _not,
  left_paren,
  right_paren,
  doc_string,
  long_value,
  double_value,
  float_value,
  string_value,
  word,
  linebreak,
  parameter_int,
  parameter_float,
  parameter_word,
  parameter_string,
  parameter_double,
  parameter_byte,
  parameter_short,
  parameter_long,
  parameter_anonymous,
  parameter_unknown,
  error,
  eof,
  none
};

[[nodiscard]] static std::string to_string(token_type type)
{
  switch (type)
  {
    case token_type::parameter_byte:
      return "byte";
    case token_type::parameter_short:
      return "short";
    case token_type::parameter_int:
      return "int";
    case token_type::parameter_long:
      return "long";
    case token_type::parameter_float:
      return "float";
    case token_type::parameter_double:
      return "double";
    case token_type::parameter_string:
      return "string";
    case token_type::parameter_word:
      return "word";
    case token_type::parameter_anonymous:
      return "anonymous";
    default:
      return std::format(
          "token_type: to_string function for type {} not implemented",
          static_cast<int>(type));
  }

  return "asdf";
}

struct token
{
  token_type type;
  std::string_view value;
  std::size_t line;
};

}  // namespace cuke::internal
