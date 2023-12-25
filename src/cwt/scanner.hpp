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

  text,
  linebreak,

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

// TODO later
class identifier
{
 public:
  virtual token_type get_identifier() = 0;
};

class scanner
{
 public:
  scanner(std::string_view source);
  [[nodiscard]] token scan_token();

  [[nodiscard]] std::string_view current_substr() const;

  [[nodiscard]] std::size_t line() const noexcept;

 private:
  char advance();
  [[nodiscard]] char peek();
  [[nodiscard]] char peek_next();
  [[nodiscard]] bool is_at_end();
  [[nodiscard]] std::size_t chars_left();
  [[nodiscard]] bool end_of_line();
  void skip_whitespace();


 private:
  std::size_t m_line;
  std::size_t m_pos;
  std::size_t m_start;

  std::string_view m_source;
};

}  // namespace cwt::details
