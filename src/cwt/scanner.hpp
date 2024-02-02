#pragma once

#include <string_view>
#include <memory>

#include "token.hpp"
#include "identifiers/identifier.hpp"
#include "identifiers/english.hpp"

namespace cwt::details
{

class scanner
{
 public:
  scanner(std::string_view source);
  [[nodiscard]] token scan_token();
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
  [[nodiscard]] bool is_alpha(char c) const noexcept;
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
  [[nodiscard]] token parameter();

 private:
  std::size_t m_line{1};
  std::size_t m_pos{0};
  std::size_t m_start{0};
  static constexpr const std::string_view m_lan_keyword{"language:"};
  std::shared_ptr<identifier> m_identifiers = std::make_shared<english>();
  std::string_view m_source;
};

}  // namespace cwt::details
