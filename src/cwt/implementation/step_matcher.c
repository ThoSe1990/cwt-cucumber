#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "step_matcher.h"
#include "value.h"
#include "object.h"
#include "vm.h"

typedef enum 
{
  INTEGER_TYPE,
  DOUBLE_TYPE,
  STRING_TYPE
} variable_type;

typedef enum {
  PH_STRING,
  PH_BYTE,
  PH_SHORT,
  PH_INT,
  PH_LONG,
  PH_DOUBLE,
  PH_FLOAT,
  PH_INVALID
} placeholder_type;

static void error(const char* step, const char* msg)
{
  fprintf(stderr, "\x1b[31m");
  fprintf(stderr, "In '%s': %s", step, msg);
  fprintf(stderr,"%s\n", msg);
  fprintf(stderr,"\x1b[0m");
}


static placeholder_type get_placehoder(const char* begin, int length)
{
  switch (*begin)
  {
  case 's': 
  {
    switch (begin[1])
    {
      case 't': return memcmp("string", begin, length) == 0 ? PH_STRING : PH_INVALID;
      case 'h': return memcmp("short", begin, length) == 0 ? PH_SHORT : PH_INVALID;
    }
  }
  case 'i': return memcmp("int", begin, length) == 0 ? PH_INT : PH_INVALID;
  case 'b': return memcmp("byte", begin, length) == 0 ? PH_BYTE : PH_INVALID;
  case 'l': return memcmp("long", begin, length) == 0 ? PH_LONG : PH_INVALID;
  case 'f': return memcmp("float", begin, length) == 0 ? PH_FLOAT : PH_INVALID;
  case 'd': return memcmp("double", begin, length) == 0 ? PH_DOUBLE : PH_INVALID;
  default: return PH_INVALID;
  }
}


typedef struct {
  const char* start;
  const char* current;
} step_scanner;

step_scanner g_defined;
step_scanner g_feature;

static bool tripple_quotes()
{
  return *g_feature.current == '"' 
    && g_feature.current[1] == '"'
    && g_feature.current[2] == '"';
}


static bool is_digit(char c)
{
  return c >= '0' && c <= '9';
}

static void init_step_scanner(const char* defined, const char* feature)
{
  g_defined.start = defined;
  g_defined.current = defined;

  g_feature.start = feature;
  g_feature.current = feature;
}

static void advance()
{
  g_defined.current++;
  g_feature.current++;
}

static bool is_at_placeholder()
{
  return *g_defined.current == '{';
}

static placeholder_type read_placeholder()
{
  g_defined.current++;
  g_defined.start = g_defined.current;
  while(*g_defined.current != '}')
  {
    g_defined.current++;
  }
  placeholder_type t = get_placehoder(g_defined.start, g_defined.current - g_defined.start);
  g_defined.current++;
  return t;
}

static void skip_quotes_and_breaks()
{
  for (;;)
  {
    switch (*g_feature.current)
    {
    case '\r':
    case '\n':
    case ' ':
    case '"':
    {
      g_feature.current++;
    }
    break; default:
      return;
    }
  }
}

static bool is_space_or_break(char c)
{
  switch (c)
  {
    case '\r':
    case '\n':
    case ' ':
    case '\0':
      return true;
  default:
    return false;
  }
}


static bool is_negative()
{
  if (*g_feature.current == '-') 
  { 
    g_feature.current++;
    return true;
  }
  else 
  {
    return false;
  }
}

static void consume_digits()
{
  while (is_digit(*g_feature.current))
  {
    g_feature.current++;
  }
}

static bool is_space_break_or_end(char c)
{
  return c == ' ' || c == '\0' || c == '\n' || c == '\r';
}

static void push_long(value_array* args, bool negative)
{
  long long long_value = strtoll(g_feature.start, NULL, 10);
  if (negative) 
  {
    long_value *= -1;
  }
  cuke_value value = LONG_VAL(long_value);
  write_value_array(args, value);
}

static void push_double(value_array* args, bool negative)
{
  double double_value = strtod(g_feature.start, NULL);
  if (negative) 
  {
    double_value *= -1;
  }
  cuke_value value = DOUBLE_VAL(double_value);
  write_value_array(args, value);
}


static bool is_variable()
{
  return *g_feature.current == '<';
}

static void trim_left()
{
  while (*g_defined.current == ' ') { g_defined.current++; }
  g_defined.start = g_defined.current;

  while (*g_feature.current == ' ') { g_feature.current++; }
  g_feature.start = g_feature.current;
}

static void trim_right()
{
  for (;;)
  {
    switch (*g_feature.current)
    {
    case '\r':
    case '\n':
    case ' ':
    {
      g_feature.current++;
    }
    break; default:
      return;
    }
  }
}

static bool is_at_end(char c)
{
  return c == '\0';
}

static bool read_variable(value_array* args, variable_type type)
{
  g_feature.current++;
  g_feature.start = g_feature.current;

  while(*g_feature.current != '>')
  {
    if (*g_feature.current == '\0') 
    {
      error(g_feature.start, "Expect '>' to end a variable.\n");
      return false;
    }
    g_feature.current++;
  }

  // TODO refactor: 
  if (args)
  {
    cuke_value value = pop();
    switch (type)
    {
      case STRING_TYPE:
      {
        if (IS_STRING(value))
        {
          write_value_array(args, value);
        }
        else 
        {
          // TODO error
          return false;
        }
      }
      break; case DOUBLE_TYPE:
      {
        if (IS_DOUBLE(value))
        {
          write_value_array(args, value);
        }
        else 
        {
          // TODO error
          return false;
        }
      }
      break; case INTEGER_TYPE:
      {
        if (IS_INT(value))
        {
          write_value_array(args, value);
        }
        else 
        {
          // TODO error
          return false;
        }
      }
    }
  }

  g_feature.current++;
  return true;
}

static void read_doc_string(value_array* args)
{
  skip_quotes_and_breaks();

  const char* start = g_feature.current;
  const char* end;
  while(!tripple_quotes())
  {
    g_feature.current++;
    if (*g_feature.current == '\n')
    {
      end = g_feature.current;
    }
  }

  skip_quotes_and_breaks();
  if (args)
  {
    cuke_value value = OBJ_VAL(copy_string(start, end - start));
    write_value_array(args, value);
  }
  return ;
}

static bool read_string(value_array* args)
{
  if (is_variable())
  {
    return read_variable(args, STRING_TYPE);
  }
  if (*g_feature.current != '"') 
  {
    error(g_feature.start, "Expect '\"' for string value.\n");
    return false;
  }
  g_feature.current++;
  g_feature.start = g_feature.current;

  while(*g_feature.current != '"')
  {
    if (*g_feature.current == '\0') 
    {
      error(g_feature.start, "Expect '\"' to end a string.\n");
      return false;
    }
    g_feature.current++;
  }

  if (args)
  {
    cuke_value value = OBJ_VAL(copy_string(g_feature.start, g_feature.current - g_feature.start));
    write_value_array(args, value);
  }

  g_feature.current++;
  return true;
}

static bool read_integer(value_array* args)
{
  if (is_variable())
  {
    return read_variable(args, INTEGER_TYPE);
  }

  bool negative = is_negative();

  g_feature.start = g_feature.current;

  consume_digits();

  if (!is_space_or_break(*g_feature.current))
  {
    error(g_feature.start, "Expect only digits in an integer value.\n");
    return false;
  }
  
  if (args)
  {
    push_long(args, negative);
  }

  return true;
}

static bool read_double(value_array* args)
{
  if (is_variable())
  {
    return read_variable(args, DOUBLE_TYPE);
  }

  bool negative = is_negative();
  g_feature.start = g_feature.current;

  consume_digits();
  if (*g_feature.current == '.')
  {
    g_feature.current++;
    consume_digits();
  }

  if (!is_space_break_or_end(*g_feature.current))
  {
    error(g_feature.start, "Expect only digits or '.' in an double value.");
    return false;
  }
  
  if (args)
  {
    push_double(args, negative);
  }

  return true;
}

static void doc_string(value_array* args)
{
  if (tripple_quotes())
  {
    read_doc_string(args);
  }
}

bool parse_step(const char* defined, const char* feature, value_array* args)
{
  init_step_scanner(defined, feature);
  trim_left();

  for (;;)
  {
    if (is_at_placeholder())
    {
      placeholder_type ph = read_placeholder();
      switch (ph) 
      {
        case PH_STRING: if (read_string(args) == false) return false;
        break; case PH_BYTE: if (read_integer(args) == false) return false;
        break; case PH_SHORT: if (read_integer(args) == false) return false;
        break; case PH_INT: if (read_integer(args) == false) return false;
        break; case PH_LONG: if (read_integer(args) == false) return false;
        break; case PH_DOUBLE: if (read_double(args) == false) return false;
        break; case PH_FLOAT: if (read_double(args) == false) return false;
        break; default: error(g_feature.start, "Invalid placeholder"); return false;
      }
    }
    
    if (is_at_end(*g_feature.current))
    {
      while (*g_defined.current == ' ') g_defined.current++;
    }

    if (is_at_end(*g_defined.current))
    {
      trim_right();
      doc_string(args);
    }
    
    if (*g_defined.current == '\0' && *g_feature.current == '\0') 
    {
      break; 
    }
    if (*g_defined.current != *g_feature.current) {
      return false;
    }
    advance();
  }
  return true;
}