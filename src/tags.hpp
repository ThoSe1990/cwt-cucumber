#pragma once

#include <vector>
#include <string>
#include <string_view>

#include "value.hpp"
#include "lexer.hpp"

namespace cuke::internal
{

struct tag_token
{
  token_type token;
  std::string value;
};

class tag_expression
{
  using value_array = std::vector<cuke::value>;

 public:
  tag_expression();
  tag_expression(std::string_view expression);

  [[nodiscard]] bool evaluate(const std::vector<std::string>& tags) const;
  [[nodiscard]] std::size_t size() const noexcept;
  [[nodiscard]] bool empty() const noexcept;
  [[nodiscard]] const std::string& expression() const noexcept;

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
  [[nodiscard]] bool contains(const std::string& tag,
                              const std::vector<std::string>& tags) const;

 private:
  std::vector<tag_token> m_out;
  std::vector<tag_token> m_operators;
  lexer m_lexer;
  int m_open_parens{0};
  std::string m_expression;
};

}  // namespace cuke::internal
