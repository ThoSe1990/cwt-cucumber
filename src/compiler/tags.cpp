#include <stack>

#include "tags.hpp"
#include "../util.hpp"

namespace cwt::details::compiler
{

tag_expression::tag_expression() : m_lexer("", bool_operators{}) {}
tag_expression::tag_expression(std::string_view expression)
    : m_lexer(expression, bool_operators{})
{
  compile();
}
std::size_t tag_expression::size() const noexcept { return m_out.size(); }
bool tag_expression::empty() const noexcept { return m_out.empty(); }
bool tag_expression::evaluate(argc n, argv tags) const
{
  if (m_out.empty() || n == 0)
  {
    return true;
  }

  std::stack<bool> stack;
  auto pop_top = [&stack]()
  {
    bool value = stack.top();
    stack.pop();
    return value;
  };

  for (const tag_token& t : m_out)
  {
    switch (t.token)
    {
      case token_type::tag:
      {
        stack.push(contains(t.value, n, tags));
      }
      break;
      case token_type::_not:
      {
        bool v = !pop_top();
        stack.push(v);
      }
      break;
      case token_type::_or:
      {
        bool rhs = pop_top();
        bool lhs = pop_top();
        stack.push(lhs || rhs);
      }
      break;
      case token_type::_and:
      {
        bool rhs = pop_top();
        bool lhs = pop_top();
        stack.push(lhs && rhs);
      }
      break;
      case token_type::_xor:
      {
        bool rhs = pop_top();
        bool lhs = pop_top();
        stack.push(lhs != rhs);
      }
      break;
      default:
        throw std::runtime_error("Unexpected token in tags.");
    }
  }

  if (stack.size() == 1) [[likely]]
  {
    return stack.top();
  }
  else [[unlikely]]
  {
    throw std::runtime_error("Shunting Yard Error: Too many values in stack");
  }
}
bool tag_expression::contains(const std::string& tag, argc n, argv tags) const
{
  for (std::size_t i = 0; i < n; ++i)
  {
    const cuke::value& current = to_value(tags, i);
    if (current.type() == cuke::value_type::string &&
        current.as<std::string>() == tag)
    {
      return true;
    }
  }
  return false;
}

const tag_token& tag_expression::operator[](std::size_t idx) const
{
  if (idx < m_out.size()) [[likely]]
  {
    return m_out.at(idx);
  }
  else [[unlikely]]
  {
    throw std::out_of_range("tag_compiler: Access is out of range.");
  }
}

tag_token tag_expression::make_token() const noexcept
{
  if (m_lexer.previous().type == token_type::tag)
  {
    return tag_token{m_lexer.previous().type,
                     std::string(m_lexer.previous().value)};
  }
  else
  {
    return tag_token{m_lexer.previous().type, std::string("")};
  }
}

void tag_expression::compile()
{
  m_lexer.advance();
  expression();

  if (m_lexer.error())
  {
    m_operators.clear();
    m_out.clear();
  }
  else if (m_open_parens != 0)
  {
    println(color::red, "Not all groupings closed or at least one grouping closed too much");
    m_operators.clear();
    m_out.clear();
  }
  else
  {
    push_remaining_operators();
  }
}
void tag_expression::expression()
{
  if (m_lexer.match(token_type::tag))
  {
    tag();
  }
  else if (m_lexer.match(token_type::left_paren))
  {
    grouping();
  }
  else if (m_lexer.match(token_type::_not))
  {
    m_operators.push_back(make_token());
    expression();
  }
  if (m_lexer.match(token_type::eof))
  {
    return;
  }
  else
  {
    m_lexer.error_at(m_lexer.current(), "Expect '@tag', '(' or 'not'.");
  }
}
void tag_expression::tag()
{
  m_out.push_back(make_token());
  if (!m_operators.empty() && m_operators.back().token == token_type::_not)
  {
    operator_to_out();
  }

  if (m_lexer.match(token_type::_and, token_type::_or, token_type::_xor))
  {
    and_or_xor();
  }
  else if (m_lexer.match(token_type::right_paren))
  {
    close_grouping();
  }
  else if (m_lexer.match(token_type::eof))
  {
    return;
  }
  else
  {
    m_lexer.error_at(m_lexer.current(), "Expect 'and' or 'or' after tag.");
  }
}

void tag_expression::left_association()
{
  if (m_operators.empty())
  {
    return;
  }

  if (m_operators.back().token == token_type::_and ||
      m_operators.back().token == token_type::_or ||
      m_operators.back().token == token_type::_xor)
  {
    operator_to_out();
  }
}

void tag_expression::and_or_xor()
{
  left_association();
  m_operators.push_back(make_token());
  expression();
}

void tag_expression::grouping()
{
  ++m_open_parens;
  m_operators.push_back(make_token());
  expression();
}
void tag_expression::close_grouping()
{
  --m_open_parens;
  for (;;)
  {
    if (m_operators.empty())
    {
      return; 
    }
    tag_token back = m_operators.back();
    m_operators.pop_back();
    if (back.token == token_type::left_paren)
    {
      break;
    }
    m_out.push_back(tag_token{back});
  }

  if (m_lexer.match(token_type::_and, token_type::_or, token_type::_xor))
  {
    and_or_xor();
  }
  else if (m_lexer.match(token_type::right_paren))
  {
    close_grouping();
  }
  else if (m_lexer.match(token_type::eof))
  {
    return;
  }
  else
  {
    m_lexer.error_at(m_lexer.current(),
                      "Expect operator or end of tags after ')'.");
  }
}

void tag_expression::push_remaining_operators()
{
  while (m_operators.size() > 0)
  {
    if (m_operators.back().token != token_type::left_paren)
    {
      operator_to_out();
    }
    else
    {
      m_operators.pop_back();
    }
  }
}

void tag_expression::operator_to_out()
{
  m_out.push_back(m_operators.back());
  m_operators.pop_back();
}

}  // namespace cwt::details::compiler
