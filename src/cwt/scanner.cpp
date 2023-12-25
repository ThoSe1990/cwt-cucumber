#include "scanner.hpp"

namespace cwt::details
{

static token make_token(const scanner& s, token_type type)
{
  token t;
  t.type = type;
  t.value = s.current_substr();
  t.line = s.line();
  return t;
}

static bool is_alpha(char c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool is_digit(char c) { return c >= '0' && c <= '9'; }

scanner::scanner(std::string_view source)
    : m_line(1), m_pos(0), m_start(0), m_source(source)
{
}

std::string_view scanner::current_substr() const
{
  return m_source.substr(m_start, m_pos - m_start);
}

std::size_t scanner::line() const noexcept { return m_line; }

char scanner::advance()
{
  ++m_pos;
  return m_source[m_pos - 1];
}
char scanner::peek() { return m_source[m_pos]; }
char scanner::peek_next()
{
  if (chars_left() >= 1)
  {
    return m_source[m_pos+1];
  }
  else
  {
    return '\0';
  }
}
bool scanner::is_at_end() { return m_pos >= m_source.size(); }
std::size_t scanner::chars_left()
{
  if (is_at_end())
  {
    return 0;
  }
  return m_source.size() - m_pos;
}
bool scanner::end_of_line()
{
  if (peek() == '\n')
  {
    return true;
  }
  else if (peek() == '\r' && peek_next() == '\n')
  {
    return true;
  }
  else
  {
    return false;
  }
}
void scanner::skip_whitespace()
{
  for (;;)
  {
    char c = peek();
    switch (c)
    {
      case ' ':
      case '\t':
        advance();
        break;
      case '#':
        while (!is_at_end() && !end_of_line())
        {
          advance();
        }
      default:
        return;
    }
  }
}

token scanner::scan_token()
{
  skip_whitespace();
  m_start = m_pos;

  if (is_at_end())
  {
    return make_token(*this, token_type::eof);
  }
  char c = advance();

  if (is_digit(c))
  {
  }
  switch (c)
  {
    case '|':
      return make_token(*this, token_type::vertical);
    case '-':
      return make_token(*this, token_type::minus);
    case '\r':
    {
      if (peek() == '\n')
      {
        advance();
        m_line++;
        return make_token(*this, token_type::linebreak);
      }
      else 
      {
        return make_token(*this, token_type::error);
      }
    }
    case '\n':
    {
      m_line++;
      return make_token(*this, token_type::linebreak);
    }
  }
  
  return make_token(*this, token_type::none);

}

}  // namespace cwt::details
