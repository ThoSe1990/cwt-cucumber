#include "scanner.hpp"
#include "identifiers/german.hpp"
#include "identifiers/spanish.hpp"

namespace cwt::details
{

static bool is_alpha(char c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool is_digit(char c) { return c >= '0' && c <= '9'; }

scanner::scanner(std::string_view source) : m_source(source)
{
  find_language();
}
scanner::scanner(std::string_view source, bool_operators)
    : m_source(source), m_identifiers(std::make_shared<bool_operators>())
{
}
void scanner::find_language()
{
  for (;;)
  {
    char c = peek();
    switch (c)
    {
      case ' ':
      case '\r':
      case '\t':
        advance();
        break;
      case '\n':
        ++m_line;
        advance();
        break;
      case '#':
      {
        bool can_change_language = true;
        advance();
        while (!is_at_end() && !end_of_line())
        {
          skip_whitespaces();
          if (can_change_language &&
              m_source.substr(m_pos).starts_with(m_lan_keyword))
          {
            m_pos += m_lan_keyword.size();
            skip_whitespaces();
            const std::size_t start = m_pos;
            while (is_alpha(peek())) advance();
            set_language(m_source.substr(start, m_pos - start));
          }
          else
          {
            can_change_language = false;
            advance();
          }
        }
        break;
      }
      default:
        return;
    }
  }
}

void scanner::set_language(std::string_view country)
{
  if (country == "de")
  {
    m_identifiers = std::make_shared<german>();
  }
  else if (country == "es")
  {
    m_identifiers = std::make_shared<spanish>();
  }
}

char scanner::advance()
{
  ++m_pos;
  return m_source[m_pos - 1];
}

token scanner::make_token(token_type type) const
{
  return token{type, m_source.substr(m_start, m_pos - m_start), m_line};
}
token scanner::make_token(token_type type, std::size_t start,
                          std::size_t end) const
{
  return token{type, m_source.substr(start, end - start), m_line};
}
token scanner::error_token(std::string_view msg) const
{
  return token{token_type::error, msg, m_line};
}

char scanner::peek() const
{
  if (!is_at_end()) [[likely]]
  {
    return m_source[m_pos];
  }
  else [[unlikely]]
  {
    return '\0';
  }
}
char scanner::peek_next() const
{
  if (chars_left() > 0)
  {
    return m_source[m_pos + 1];
  }
  else
  {
    return '\0';
  }
}
bool scanner::is_alpha(char c) const noexcept
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}
bool scanner::is_at_end() const { return m_pos >= m_source.size(); }
bool scanner::three_consecutive(const char c) const
{
  if (chars_left() < 2)
  {
    return false;
  }
  else
  {
    return m_source[m_pos] == c && m_source[m_pos + 1] == c &&
           m_source[m_pos + 2] == c;
  }
}
std::size_t scanner::chars_left() const
{
  if (is_at_end())
  {
    return 0;
  }
  return m_source.size() - 1 - m_pos;
}
bool scanner::end_of_line() const
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
void scanner::skip_whitespaces()
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
      default:
        return;
    }
  }
}
void scanner::skip()
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

token scanner::number()
{
  bool is_double = false;
  while (is_digit(peek()))
  {
    advance();
  }
  if (peek() == '.')
  {
    is_double = true;
    advance();
  }

  while (is_digit(peek()))
  {
    advance();
  }

  return is_double ? make_token(token_type::double_value)
                   : make_token(token_type::long_value);
}

token scanner::tag()
{
  while (is_alpha(peek()) || is_digit(peek()))
  {
    advance();
  }
  return make_token(token_type::tag);
}
token scanner::variable()
{
  while (peek() != '>')
  {
    if (is_at_end() || end_of_line())
    {
      return error_token("Expect '>' after variable.");
    }
    advance();
  }
  advance();
  return make_token(token_type::variable);
}
token scanner::string()
{
  while (peek() != '"' && !is_at_end())
  {
    if (peek() == '\n')
    {
      return error_token("Unexpected linebreak in string value.");
    }
    advance();
  }

  if (is_at_end())
  {
    return error_token("Unterminated string.");
  }
  advance();
  return make_token(token_type::string_value);
}
token scanner::doc_string()
{
  advance();
  advance();

  while (!three_consecutive('"') && !three_consecutive('`'))
  {
    if (end_of_line())
    {
      m_line++;
    }
    advance();

    if (is_at_end())
    {
      return error_token("Unterminated doc string.");
    }
  }

  skip();
  advance();
  advance();
  advance();

  return make_token(token_type::doc_string, m_start, m_pos);
}

token scanner::word()
{
  while (is_alpha(peek()))
  {
    advance();
  }
  return make_token(token_type::word);
}

token scanner::parameter()
{
  while (peek() != '}')
  {
    if (is_at_end() || end_of_line())
    {
      return error_token("Expect '}' after parameter.");
    }
    advance();
  }
  advance();

  if (m_source.substr(m_start, m_pos - m_start).starts_with("{int}"))
  {
    return make_token(token_type::parameter_int);
  }
  else if (m_source.substr(m_start, m_pos - m_start).starts_with("{float}"))
  {
    return make_token(token_type::parameter_float);
  }
  else if (m_source.substr(m_start, m_pos - m_start).starts_with("{word}"))
  {
    return make_token(token_type::parameter_word);
  }
  else if (m_source.substr(m_start, m_pos - m_start).starts_with("{string}"))
  {
    return make_token(token_type::parameter_string);
  }
  else if (m_source.substr(m_start, m_pos - m_start).starts_with("{double}"))
  {
    return make_token(token_type::parameter_double);
  }
  else if (m_source.substr(m_start, m_pos - m_start).starts_with("{byte}"))
  {
    return make_token(token_type::parameter_byte);
  }
  else if (m_source.substr(m_start, m_pos - m_start).starts_with("{short}"))
  {
    return make_token(token_type::parameter_short);
  }
  else if (m_source.substr(m_start, m_pos - m_start).starts_with("{long}"))
  {
    return make_token(token_type::parameter_long);
  }
  else
  {
    return make_token(token_type::parameter_unknown);
  }
}
void scanner::reset()
{
  m_start = 0;
  m_pos = 0;
}
token scanner::scan_token()
{
  skip();
  m_start = m_pos;

  if (is_at_end())
  {
    return make_token(token_type::eof);
  }
  char c = advance();

  if (is_digit(c))
  {
    return number();
  }
  switch (c)
  {
    case '|':
      return make_token(token_type::vertical);
    case '(':
      return make_token(token_type::left_paren);
    case ')':
      return make_token(token_type::right_paren);
    case '{':
      return parameter();
    case '-':
      return make_token(token_type::minus);
    case '*':
      return make_token(token_type::step);
    case '@':
      return tag();
    case '<':
      return variable();
    case '`':
    {
      if (peek() == '`' && peek_next() == '`')
      {
        return doc_string();
      }
      else
      {
        return error_token("Expect doc string after '`'.");
      }
    }
    case '"':
    {
      if (peek() == '"' && peek_next() == '"')
      {
        return doc_string();
      }
      else
      {
        return string();
      }
    }
    case '\r':
    {
      if (peek() == '\n')
      {
        advance();
        m_line++;
        return make_token(token_type::linebreak);
      }
      else
      {
        return make_token(token_type::error);
      }
    }
    case '\n':
    {
      m_line++;
      return make_token(token_type::linebreak);
    }
  }

  auto [identifier, length] =
      m_identifiers->get_token(m_source.substr(m_pos - 1));
  if (identifier != token_type::none)
  {
    m_pos += length - 1;
    return make_token(identifier);
  }

  return word();
}

}  // namespace cwt::details
