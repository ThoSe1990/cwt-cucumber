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

  error,
  eof,
  none
};

enum class country_code
{
  en,
  es,
  de
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
  [[nodiscard]] country_code langauge() const noexcept;

 private:
  char advance();
  void skip();
  void skip_whitespaces();
  void find_language();
  void language(std::string_view country);
  [[nodiscard]] char peek() const;
  [[nodiscard]] char peek_next() const;
  [[nodiscard]] std::size_t chars_left() const;
  [[nodiscard]] bool is_at_end() const;
  [[nodiscard]] bool tripple_quotes() const;
  [[nodiscard]] bool end_of_line() const;
  [[nodiscard]] token make_token(token_type type) const;
  [[nodiscard]] token make_token(token_type type, std::size_t start,
                                 std::size_t end) const;
  [[nodiscard]] token error_token(std::string_view msg) const;
  [[nodiscard]] token number();
  [[nodiscard]] token tag();
  [[nodiscard]] token variable();
  [[nodiscard]] token string();
  [[nodiscard]] token doc_string();
  [[nodiscard]] token word();

 private:
  std::size_t m_line{1};
  std::size_t m_pos{0};
  std::size_t m_start{0};
  country_code m_language{country_code::en};
  static constexpr const std::string_view m_lan_keyword{"language:"};
  std::string_view m_source;
};

}  // namespace cwt::details
