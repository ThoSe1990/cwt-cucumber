#include <iostream>

#include "lexer.hpp"

#include "util.hpp"

namespace cwt::details
{
lexer::lexer(const file& f) : m_scanner(f.content), m_filepath(f.path) {}
lexer::lexer(std::string_view source) : m_scanner(source) {}
lexer::lexer(std::string_view source, bool_operators)
    : m_scanner(source, bool_operators{})
{
}

const token& lexer::current() const noexcept { return m_current; }
const token& lexer::previous() const noexcept { return m_previous; }
const std::string& lexer::filepath() const noexcept { return m_filepath; }
bool lexer::error() const noexcept { return m_error; }

void lexer::error_at(const token& t, std::string_view msg) noexcept
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

bool lexer::check(token_type type) const noexcept
{
  return m_current.type == type;
}
bool lexer::match(token_type type) noexcept
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
std::size_t lexer::advance_to(token_type type)
{
  std::size_t count = 0;
  while (!check(type))
  {
    ++count;
    advance();
    if (check(token_type::eof))
    {
      error_at(m_current, "Unexpected end of file.");
      return count;
    }
  }
  return count;
}

std::vector<token> lexer::collect_tokens_to(token_type type)
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

void lexer::consume(token_type type, std::string_view msg)
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
void lexer::skip_linebreaks()
{
  while (match(token_type::linebreak))
  {
  }
}
void lexer::advance()
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
