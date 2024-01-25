#pragma once

#include <string_view>

namespace cwt::details
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
  doc_string,
  long_value,
  double_value,
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

  error,
  eof,
  none
};

struct token
{
  token_type type;
  std::string_view value;
  std::size_t line;
};

}  // namespace cwt::details