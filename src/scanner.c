#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

typedef struct {
  const char* start;
  const char* current;
  int line;
} scanner_t;

scanner_t scanner; 

void init_scanner(const char* source)
{
  scanner.start = source;
  scanner.current = source; 
  scanner.line = 1; 
}

static bool is_alpha(char c)
{
  return  (c >= 'a' && c <= 'z') ||
          (c >= 'A' && c <= 'Z') ||
          c == '_';
}

static bool is_digit(char c)
{
  return c >= '0' && c <= '9';
}

static bool is_at_end()
{
  return *scanner.current == '\0';
}

static char advance() 
{
  scanner.current++;
  return scanner.current[-1];
}

static char peek() 
{
  return *scanner.current;
}

static char peek_next()
{
  if (is_at_end()) 
  {
    return '\0';
  }
  else 
  {
    return scanner.current[1];
  }
}

static token make_token(token_type type)
{
  token t; 
  t.type = type;
  t.start = scanner.start;
  t.length = (int)(scanner.current - scanner.start);
  t.line = scanner.line;
  return t;
}
static token error_token(const char* msg)
{
  token t;
  t.type = TOKEN_ERROR;
  t.start = msg;
  t.length = (int)strlen(msg);
  t.line = scanner.line;
  return t;
}

// TODO macros for unix/windows fine too? 
static bool end_of_line()
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
static void skip_whitespace()
{
  for(;;)
  {
    char c = peek();
    switch (c)
    {
      case ' ':
      case '\t':
        advance(); break;
      case '#':
        while (!end_of_line() && !is_at_end()) 
        { 
          advance(); 
        }
      default:
        return;
    }
  }
}

static token_type check_keyword(int start, int length, const char* rest, token_type type)
{
  if (scanner.current - scanner.start == start + length && 
    memcmp(scanner.start + start, rest, length) == 0 )
  {
    return type;
  }
  else 
  {
    return NO_TOKEN;
  }
}
static token_type check_keyword_with_colon(int start, int length, const char* rest, token_type type)
{
  if (check_keyword(start, length, rest, type) == type && peek() == ':')
  {
    advance();
    return type;
  }
  else 
  {
    return NO_TOKEN;
  }
}

static token_type identifier_type()
{
  switch (scanner.start[0])
  {
    case 'F': return check_keyword_with_colon(1,6, "eature", TOKEN_FEATURE);
    case 'S': return check_keyword_with_colon(1,7, "cenario", TOKEN_SCENARIO);
    case 'G': return check_keyword(1,4, "iven", TOKEN_STEP);
    case 'W': return check_keyword(1,3, "hen", TOKEN_STEP);
    case 'T': return check_keyword(1,3, "hen", TOKEN_STEP);
    default: return NO_TOKEN;
  }
}


static token_type identifier()
{
  while (is_alpha(peek())) 
  {
    advance();
  }
  return identifier_type();
}


static bool match(char expected)
{
  if (is_at_end()) { return false; }
  if (*scanner.current != expected) { return false; }
  scanner.current++;
  return true; 
}


static bool whitespace()
{
  return peek() == ' ';
}


static token number()
{
  while (is_digit(peek())) { advance(); }
  if (peek() == '.' && is_digit(peek_next()))
  {
    advance();
    while(is_digit(peek())) { advance(); }
  }
  return make_token(TOKEN_INT);
}

static token string()
{
  while (peek() != '"' && !is_at_end())
  {
    if (peek() == '\n')
    {
      scanner.line++;
    }
    advance();
  }

  if (is_at_end()) 
  {
    return error_token("Unterminated string.");
  }
  advance();
  return make_token(TOKEN_STRING);
}

static token text()
{
  while (!whitespace() && !end_of_line() && !is_at_end())
  {
    advance();
  }
  return make_token(TOKEN_TEXT);
}



token scan_token()
{
  skip_whitespace();

  scanner.start = scanner.current;

  if (is_at_end()) 
  { 
    return make_token(TOKEN_EOF); 
  }
  
  char c = advance();

  token_type current = identifier();
  if (current != NO_TOKEN) 
  {
    return make_token(current);
  }

  if (is_digit(c)) 
  {
    return number();
  }

  switch(c)
  {
    case '|': return make_token(TOKEN_VERTICAL);
    case '"': 
    {
      if (peek() == '"' && peek_next() == '"')
      {
        // TODO
        // return doc_string();
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
        scanner.line++;
        return make_token(TOKEN_LINEBREAK);
      }
    }
    case '\n': 
    {
      scanner.line++;
      return make_token(TOKEN_LINEBREAK);
    }
    // case '<': return variable();
  }

  return text(); 
}