#include <iostream>

#include "parser.hpp"

#include "util.hpp"

namespace cwt::details
{
parser::parser(const file& f) : m_scanner(f.content), m_filepath(f.path) {}
parser::parser(std::string_view source) : m_scanner(source) {}
parser::parser(std::string_view source, bool_operators)
    : m_scanner(source, bool_operators{})
{
}

const token& parser::current() const noexcept { return m_current; }
const token& parser::previous() const noexcept { return m_previous; }

bool parser::error() const noexcept { return m_error; }

void parser::error_at(const token& t, std::string_view msg) noexcept
{
  std::string prefix("");
  if (m_filepath.empty())
  {
    prefix += std::format("[line {}] Error ", t.line);
  }
  else
  {
    prefix += std::format("{}:{}: Error ", m_filepath, t.line);
  }

  if (t.type == token_type::eof)
  {
    prefix += "at end";
  }
  else if (t.type == token_type::error)
  {
    // nothing
  }
  else
  {
    prefix += std::format("at '{}'", t.value);
  }

  println(color::red, std::format("{}: {}", prefix, msg));
  m_error = true;
}

bool parser::check(token_type type) const noexcept
{
  return m_current.type == type;
}
bool parser::match(token_type type) noexcept
{
  if (check(type))
  {
    advance();
    return true;
  }
  else
  {
    return false;
  }
}
void parser::advance_to(token_type type)
{
  while (!check(type))
  {
    advance();
    if (check(token_type::eof))
    {
      error_at(m_current, "Unexpected end of file.");
      return;
    }
  }
}

std::vector<token> parser::collect_tokens_to(token_type type)
{
  std::vector<token> result;
  while (!check(type))
  {
    result.push_back(m_current);
    advance();
    if (check(token_type::eof))
    {
      error_at(m_current, "Unexpected end of file.");
      break;
    }
  }
  return result;
}

void parser::consume(token_type type, std::string_view msg)
{
  if (m_current.type == type)
  {
    advance();
  }
  else
  {
    error_at(m_current, msg);
  }
}
void parser::skip_linebreaks()
{
  while (match(token_type::linebreak))
  {
  }
}
void parser::advance()
{
  m_previous = m_current;
  for (;;)
  {
    m_current = m_scanner.scan_token();
    if (m_current.type != token_type::error)
    {
      break;
    }
    error_at(m_current, m_current.value);
  }
}

}  // namespace cwt::details
