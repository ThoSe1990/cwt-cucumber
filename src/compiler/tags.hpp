#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <string_view>

#include "../value.hpp"
#include "../lexer.hpp"

namespace cwt::details::compiler
{

struct tag_token
{
  token_type token;
  std::string value;
};

class tag_expression
{
 public:
  tag_expression();
  tag_expression(std::string_view expression);

  [[nodiscard]] bool evaluate(argc n, argv tags) const;
  [[nodiscard]] std::size_t size() const noexcept;
  [[nodiscard]] bool empty() const noexcept;

  const tag_token& operator[](std::size_t idx) const;

 private:
  [[nodiscard]] tag_token make_token() const noexcept;
  void compile();
  void expression();
  void tag();
  void and_or_xor();
  void left_association();
  void grouping();
  void close_grouping();
  void push_remaining_operators();

  void operator_to_out();
  [[nodiscard]] bool contains(const std::string& tag, argc n, argv tags) const;

 private:
  std::vector<tag_token> m_out;
  std::vector<tag_token> m_operators;
  lexer m_lexer;
  int m_open_parens{0};
};

}  // namespace cwt::details::compiler
