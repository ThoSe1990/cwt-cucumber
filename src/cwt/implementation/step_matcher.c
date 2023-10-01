#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "step_matcher.h"
#include "value.h"
#include "object.h"

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

static bool is_digit(char c)
{
  return c >= '0' && c <= '9';
}

typedef struct {
  const char* start;
  const char* current;
} step_scanner;

step_scanner g_defined;
step_scanner g_feature;

static void init_step_scanner(const char* defined, const char* feature)
{
  g_defined.start = defined;
  g_defined.current = defined;

  g_feature.start = feature;
  g_feature.current = feature;
}

static void skip_whitespaces()
{
  while (*g_defined.current == ' ') { g_defined.current++; }
  g_defined.start = g_defined.current;

  while (*g_feature.current == ' ') { g_feature.current++; }
  g_feature.start = g_feature.current;
}

static void advance()
{
  g_defined.current++;
  g_feature.current++;
}

static bool not_at_end()
{
  return *g_defined.current != '\0' 
    && *g_feature.current != '\0';
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

// TODO refactor, lot of DRY in this read_* functions.. 

static bool read_string(value_array* args)
{
  if (*g_feature.current != '"') 
  {
    fprintf(stderr, "Expect '\"' for string value.\n");
    return false;
  }
  g_feature.current++;
  g_feature.start = g_feature.current;

  while(*g_feature.current != '"')
  {
    g_feature.current++;
  }

  if (args)
  {
    cwtc_value value = OBJ_VAL(copy_string(g_feature.start, g_feature.current - g_feature.start));
    write_value_array(args, value);
  }

  g_feature.current++;
  return true;
}

static bool read_integer(value_array* args)
{
  bool negative = false;
  if (*g_feature.current == '-') 
  {
    g_feature.current++;
    negative = true;
  }
  g_feature.start = g_feature.current;

  while (is_digit(*g_feature.current))
  {
    g_feature.current++;
  }
  if (*g_feature.current != ' ' && *g_feature.current != '\0')
  {
    fprintf(stderr, "Expect only digits in an integer value.");
    return false;
  }
  
  if (args)
  {
    long long long_value = strtold(g_feature.start, NULL);
    if (negative) 
    {
      long_value *= -1;
    }
    cwtc_value value = LONG_VAL(long_value);
    write_value_array(args, value);
  }

  return true;
}

static bool read_double(value_array* args)
{
  bool negative = false;
  if (*g_feature.current == '-') 
  {
    g_feature.current++;
    negative = true;
  }
  g_feature.start = g_feature.current;

  while (is_digit(*g_feature.current))
  {
    g_feature.current++;
  }
  if (*g_feature.current == '.')
  {
    g_feature.current++;
  }
  while (is_digit(*g_feature.current))
  {
    g_feature.current++;
  }

  if (*g_feature.current != ' ' && *g_feature.current != '\0')
  {
    fprintf(stderr, "Expect only digits or '.' in an double value.");
    return false;
  }
  
  if (args)
  {
    double double_value = strtod(g_feature.start, NULL);
    if (negative) 
    {
      double_value *= -1;
    }
    cwtc_value value = DOUBLE_VAL(double_value);
    write_value_array(args, value);
  }

  return true;
}

bool parse_step(const char* defined, const char* feature, value_array* args)
{
  init_step_scanner(defined, feature);
  skip_whitespaces();

  while(not_at_end())
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
      }
    }
    while (*g_defined.current == '\0' && *g_feature.current == ' ') 
    {
      g_feature.current++;
    }
    while (*g_defined.current == ' ' && *g_feature.current == '\0') 
    {
      g_defined.current++;
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
