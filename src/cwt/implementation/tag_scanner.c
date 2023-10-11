#include <stdbool.h>

#include "tag_scanner.h"

typedef struct {
  const char* start;
  const char* current;
} tag_scanner_t;

tag_scanner_t scanner; 


void init_tag_scanner(const char* tags)
{
  scanner.current = tags;
  scanner.start = tags;
}

static bool is_lower_case_alpha(char c)
{
  return  (c >= 'a' && c <= 'z');
}

static bool is_at_end()
{
  return *scanner.current == '\0';
}
static bool is_whitespace()
{
  return *scanner.current == ' ';
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
static tag_token make_token(tag_token_type type)
{
  tag_token t; 
  t.type = type;
  t.start = scanner.start;
  t.length = (int)(scanner.current - scanner.start);
  return t;
}
static tag_token error_tag_token(const char* msg)
{
  tag_token t;
  t.type = TAG_TOKEN_ERROR;
  t.start = msg;
  t.length = (int)strlen(msg);
  return t;
}
static void skip_whitespace()
{
  for(;;)
  {
    char c = peek();
    switch (c)
    {
      case ' ':
      case '\r':
      case '\t':
      case '\n':
        advance();
        break;
      default:
        return;
    }
  }
}

static tag_token_type check_keyword(int start, int length, const char* rest, tag_token_type type)
{
  if (  scanner.current - scanner.start == start + length && 
        memcmp(scanner.start + start, rest, length) == 0 )
  {
    return  type;
  } 
  else 
  {
    return TAG_TOKEN_ERROR;
  }
}

static tag_token_type identifier_type()
{
  switch (scanner.start[0])
  {
    case 'a': return check_keyword(1,2, "nd", TAG_TOKEN_AND);
    case 'o': return check_keyword(1,1, "r", TAG_TOKEN_OR);
    case 'n': return check_keyword(1,2, "ot", TAG_TOKEN_NOT);
    default: return TAG_TOKEN_ERROR;
  }
}

static tag_token identifier()
{
  while (!is_whitespace() && !is_at_end()) 
  {
    advance();
  }
  tag_token_type t = identifier_type();
  return t != TAG_TOKEN_ERROR 
    ?  make_token(identifier_type())
    : error_tag_token("invalid identifier in tags");
}
static tag_token tag()
{
  while (!is_whitespace() && !is_at_end()) 
  {
    advance();
  }
  return make_token(TAG_TOKEN_TAG);
}
tag_token scan_tag_token()
{
  skip_whitespace();

  scanner.start = scanner.current;

  if (is_at_end())
  {
    return make_token(TAG_TOKEN_EOL);
  }

  char c = advance();

  if (is_alpha())
  {
    return identifier();
  }

  switch (c)
  {
  case '(' : return make_token(TAG_TOKEN_LEFT_PAREN);
  case ')' : return make_token(TAG_TOKEN_RIGHT_PAREN);
  case '@' : return tag();
  }

  return error_tag_token("Invalid tags given.");
}

#include "value.h"

typedef struct {
  tag_token current;
  tag_token previous; 
  bool had_error;
} tag_parser_t;

tag_parser_t tag_parser; 

static void advance()
{
  tag_parser.previous = tag_parser.current;

  for (;;) 
  {
    tag_parser.current = scan_tag_token();
    if (tag_parser.current.type != TAG_TOKEN_ERROR) 
    {
      break;
    }
    // TODO error msg 
    // error_at_current(parser.current.start);
  }
}

value_array* operators;
value_array* output;

static void expression()
{
  if 
}

value_array* get_tag_condition(const char* tags)
{
  init_tag_scanner(tags);
  
  init_value_array(&output);
  init_value_array(&operators);

  advance();
  expression();
}