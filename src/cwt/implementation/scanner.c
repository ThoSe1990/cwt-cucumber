#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

typedef struct {
  const char* start;
  const char* current;
  int line;
  const char* filename;
} scanner_t;

scanner_t scanner; 

void init_scanner(const char* source, const char* filename)
{
  scanner.start = source;
  scanner.current = source; 
  scanner.line = 1; 
  scanner.filename = filename;
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

static token make_explicit_token(token_type type, const char* start, int length)
{
  token t; 
  t.type = type;
  t.start = start;
  t.length = length;
  t.line = scanner.line;
  return t;
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
static void skip_whitespace_and_linebreaks()
{
  for(;;)
  {
    char c = peek();
    switch (c)
    {
      case ' ':
      case '\r':
      case '\t':
        advance(); break;
      case '\n':
        scanner.line++;
        advance();
        break;
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

bool is_same(const char* s1, const char* s2, int length)
{
  for (int i = 0 ; i < 0 ; i++)
  {
    if (s1[i] != s2[i]) 
    {
      return false; 
    }
  }
  return true;
}

static token_type check_keyword(int start, int length, const char* rest, token_type type)
{
  if (scanner.current - scanner.start == start + length && 
    is_same(scanner.start + start, rest, length))
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

void next_word()
{
  while (is_alpha(peek())) 
  {
    advance();
  }
}

static token_type scenario() 
{
  token_type t = check_keyword_with_colon(1,7, "cenario", TOKEN_SCENARIO);
  if (t == NO_TOKEN)
  {
    advance();
    next_word();
    t = check_keyword_with_colon(1,15, "cenario Outline", TOKEN_SCENARIO_OUTLINE);
  }
  return t;
}

static token_type identifier_type()
{
  switch (scanner.start[0])
  {
    case 'F': return check_keyword_with_colon(1,6, "eature", TOKEN_FEATURE);
    case 'S': return scenario();
    case 'E': return check_keyword_with_colon(1,7, "xamples", TOKEN_EXAMPLES);
    case 'G': return check_keyword(1,4, "iven", TOKEN_STEP);
    case 'W': return check_keyword(1,3, "hen", TOKEN_STEP);
    case 'T': return check_keyword(1,3, "hen", TOKEN_STEP);
    case 'A': return check_keyword(1,2, "nd", TOKEN_STEP);
    case 'B': return check_keyword(1,2, "ut", TOKEN_STEP);
    case '*': return TOKEN_STEP;
    default: return NO_TOKEN;
  }
}


static token_type identifier()
{
  next_word();
  return identifier_type();
}


static bool match(char expected)
{
  if (is_at_end()) { return false; }
  if (*scanner.current != expected) { return false; }
  scanner.current++;
  return true; 
}


static bool text_delimiter()
{
  return peek() == ' ' || peek() == '|';
}

static token variable()
{
  while (peek() != '>')
  {
    if (peek() == '\n')
    {
      return error_token("Expect '>' after variable.");
    }
    advance();
  }
  advance();
  return make_token(TOKEN_VAR);
}

static token number()
{
  bool is_double = false;
  while (is_digit(peek())) { advance(); }
  if (peek() == '.' && is_digit(peek_next()))
  {
    is_double = true;
    advance();
    while(is_digit(peek())) { advance(); }
  }
  return is_double ? make_token(TOKEN_DOUBLE) : make_token(TOKEN_INT);
}

static token string()
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
  return make_token(TOKEN_STRING);
}

static token doc_string()
{
  const char* start = scanner.current;
  const char* end;
  while (peek() != '"' && peek_next() != '"' && !is_at_end())
  {
    if (peek() == '\n')
    {
      end = scanner.current; 
      if (*end == '\r') end--;
      scanner.line++;
    }
    advance();
  }

  skip_whitespace();
  advance();
  advance();
  advance();
  skip_whitespace_and_linebreaks();
  
  if (is_at_end()) 
  {
    return error_token("Unterminated doc string.");
  }
  return make_explicit_token(TOKEN_DOC_STRING, start, end-start);
}

static token text()
{
  while (!text_delimiter() && !end_of_line() && !is_at_end())
  {
    advance();
  }
  return make_token(TOKEN_TEXT);
}

const char* filename()
{
  return scanner.filename;
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
    case '-': return make_token(TOKEN_MINUS);
    case '<': return variable();
    case '"': 
    {
      if (peek() == '"' && peek_next() == '"')
      {
        advance();
        advance();
        advance();
        skip_whitespace_and_linebreaks();
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
        scanner.line++;
        return make_token(TOKEN_LINEBREAK);
      }
    }
    case '\n': 
    {
      scanner.line++;
      return make_token(TOKEN_LINEBREAK);
    }
  }

  return text(); 
}