#include <stdbool.h>
#include <stdio.h>

#include "tag_scanner.h"
#include "object.h"

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

static bool right_paren()
{
  return *scanner.current == ')'; 
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
static bool is_alpha(char c)
{
  return  (c >= 'a' && c <= 'z');
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
static tag_token make_tag()
{
  while (!is_whitespace() && !right_paren() && !is_at_end())
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

  if (is_alpha(c))
  {
    return identifier();
  }

  switch (c)
  {
  case '(' : return make_token(TAG_TOKEN_LEFT_PAREN);
  case ')' : return make_token(TAG_TOKEN_RIGHT_PAREN);
  case '@' : return make_tag();
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

static void next_token()
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

static bool check(tag_token_type type)
{
  return tag_parser.current.type == type;
}

static bool match(tag_token_type type)
{
  if (check(type)) 
  { 
    next_token();
    return true;
  }
  else 
  {
    return false; 
  }
}


typedef struct 
{
  cuke_value out[TAGS_RPN_MAX];
  cuke_value* out_top;
  cuke_value operators[TAGS_RPN_MAX];
  cuke_value* operators_top;
} tag_values_t;

tag_values_t tag_values;

void push_operator(cuke_value value)
{
  *tag_values.operators_top = value;
  tag_values.operators_top++;
}
cuke_value pop_operator()
{
  tag_values.operators_top--;
  return *tag_values.operators_top;
}

void push_out(cuke_value value)
{
  *tag_values.out_top = value;
  tag_values.out_top++;
}
cuke_value pop_out()
{
  tag_values.out_top--;
  return *tag_values.out_top;
}


static void error(const char* msg)
{
  fprintf(stderr, "\x1b[31m");
  fprintf(stderr, "Tag-Error: %s\n", msg);
  fprintf(stderr,"\x1b[0m");
  tag_parser.had_error = true;
}


static void consume(tag_token_type type, const char* msg)
{
  if (tag_parser.current.type == type) 
  {
    next_token();
    return;
  }

  error(msg);
}

static void expression();

static void close_grouping()
{
  for(;;)
  {
    cuke_value operator = pop_operator();
    if (*AS_CSTRING(operator) == '(') 
    {
      break;
    }
    push_out(operator);
  }

  if (match(TAG_TOKEN_AND) || match(TAG_TOKEN_OR))
  {
    push_operator(OBJ_VAL(copy_string(tag_parser.previous.start, tag_parser.previous.length)));
    expression();
  }
  else if (match(TAG_TOKEN_EOL))
  {
    return;
  }
  else 
  {
    error("Expect operator or end of tags after ')'");
  }
}

static void grouping()
{
  push_operator(OBJ_VAL(copy_string(tag_parser.previous.start, tag_parser.previous.length)));
  expression();
}

static void tag()
{
  push_out(OBJ_VAL(copy_string(tag_parser.previous.start, tag_parser.previous.length)));
  if (match(TAG_TOKEN_AND) || match(TAG_TOKEN_OR))
  {
    push_operator(OBJ_VAL(copy_string(tag_parser.previous.start, tag_parser.previous.length)));
    expression();
  }
  else if (match(TAG_TOKEN_RIGHT_PAREN))
  {
    close_grouping();
  }
  else if (match(TAG_TOKEN_EOL))
  {
    return ;
  }
  else 
  {
    error("Expect 'and' or 'or' after tag.");
  }
}

static void expression()
{
  if (match(TAG_TOKEN_TAG))
  {
    tag();
  }
  else if (match(TAG_TOKEN_LEFT_PAREN))
  {
    grouping();  
  }
  else if (match(TAG_TOKEN_NOT))
  {
    push_operator(OBJ_VAL(copy_string(tag_parser.previous.start, tag_parser.previous.length)));
    expression();
  }
  else if (match(TAG_TOKEN_EOL))
  {
    return ;
  }
  else 
  {
    error("Expect '@tag', '(' or 'not'.");
  }
}

static void reset_tag_values()
{
  tag_values.operators_top = tag_values.operators;
  tag_values.out_top = tag_values.out;
}

int tags_to_rpn_stack(const char* tags, cuke_value* result)
{
  init_tag_scanner(tags);

  reset_tag_values();

  tag_parser.had_error = false;
  next_token();
  expression();

  while (tag_values.operators_top != tag_values.operators)
  {
    push_out(pop_operator());
  }
  int size = tag_values.out_top - tag_values.out;
  for (int i = 0 ; i < size ; i++)
  {
    result[i] = tag_values.out[i];
  }
  return size;
}