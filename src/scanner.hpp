#pragma once

#include <string_view>
#include <memory>

#include "token.hpp"
#include "identifiers/identifier.hpp"
#include "identifiers/english.hpp"
#include "identifiers/bool_operators.hpp"

namespace cuke::internal
{

class scanner
{
 public:
  scanner(std::string_view source);
  scanner(std::string_view source, bool_operators);
  [[nodiscard]] token scan_token();
  [[nodiscard]] token scan_word(std::size_t length);
  [[nodiscard]] token scan_token_until(const token& end);
  void reset();

 private:
  char advance();
  void skip();
  void skip_whitespaces();
  void find_language();
  void set_language(std::string_view country);
  [[nodiscard]] char peek() const;
  [[nodiscard]] char peek_next() const;
  [[nodiscard]] std::size_t chars_left() const;
  [[nodiscard]] bool is_at_end() const;
  [[nodiscard]] bool is_whitespace() const;
  [[nodiscard]] bool three_consecutive(const char c) const;
  [[nodiscard]] bool end_of_line() const;
  [[nodiscard]] token make_token(token_type type) const;
  [[nodiscard]] token make_token(token_type type, std::size_t length) const;
  [[nodiscard]] token make_token(token_type type, std::size_t start,
                                 std::size_t end) const;
  [[nodiscard]] token error_token(std::string_view msg) const;
  [[nodiscard]] token number();
  [[nodiscard]] token tag();
  [[nodiscard]] token variable();
  [[nodiscard]] token string();
  [[nodiscard]] token doc_string();
  [[nodiscard]] token word();
  [[nodiscard]] token parameter();
  [[nodiscard]] std::string_view make_string_view_here(std::size_t length);

 private:
  std::size_t m_line{1};
  std::size_t m_pos{0};
  std::size_t m_start{0};
  static constexpr const std::string_view m_lan_keyword{"language:"};
  std::shared_ptr<identifier> m_identifiers = std::make_shared<english>();
  std::string_view m_source;
};

}  // namespace cuke::internal
