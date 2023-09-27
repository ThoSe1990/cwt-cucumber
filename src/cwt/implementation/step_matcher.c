#include "step_matcher.h"

#include <stdio.h>
#include <string.h>

typedef enum {
  PH_STRING,
  PH_BYTE,
  PH_INT,
  PH_SHORT,
  PH_DOUBLE,
  PH_FLOAT,
  PH_INVALID
} placeholder_type;

placeholder_type get_placehoder(const char* begin, const char* end)
{
  switch (*begin)
  {
  case 's': 
  {
    switch (begin[1])
    {
      case 't': return memcmp("tring", begin+1, end-begin) == 0 ? PH_STRING : PH_INVALID;
      case 'h': return memcmp("hort", begin+1, end-begin) == 0 ? PH_SHORT : PH_INVALID;
    }
  }
  case 'i': return memcmp("nt", begin+1, end-begin) == 0 ? PH_INT : PH_INVALID;
  case 'b': return memcmp("yte", begin+1, end-begin) == 0 ? PH_BYTE : PH_INVALID;
  case 'f': return memcmp("loat", begin+1, end-begin) == 0 ? PH_FLOAT : PH_INVALID;
  case 'd': return memcmp("ouble", begin+1, end-begin) == 0 ? PH_DOUBLE : PH_INVALID;
  default: return PH_INVALID;
  }
}

// TODO get never closing bracket!
void advance_after(char c, const char** str)
{
  while(c != **str)
  {
    (*str)++;
    if (c == '\0' || c == '\n') 
    {
      fprintf(stderr, "Missing %c in StepLine.\n", c);
      return;
    }
  }
  (*str)++;
}

placeholder_type placeholder(const char** str)
{
  (*str)++;
  const char* var_begin = *str;
  advance_after('}',str);
  const char* var_end = *str-2;
  return get_placehoder(var_begin, var_end);
}

bool is_digit(char c)
{
  return c >= '0' && c <= '9';
}

bool double_(const char** str)
{
  if (**str == '-') 
  {
    (*str)++;
  }
  while (is_digit(**str)) 
  {
    (*str)++;
  }
  if (**str == ' ') { return true;}
  if (**str != '.') { return false; }
  (*str)++;
  while (is_digit(**str)) 
  {
    (*str)++;
  }
  return **str == ' ' || **str == '\0';
}

bool integer(const char** str)
{
  if (**str == '-') 
  {
    (*str)++;
  }
  while (is_digit(**str))
  {
    (*str)++;
  }
  return **str == ' ' || **str == '\0';
}

bool string(const char** str) 
{
  if (**str != '"') 
  {
    fprintf(stderr, "Expect '\"' for string value.\n");
    return false;
  }
  (*str)++;
  advance_after('"', str);
  return true;
}

bool is_step(const char* defined, const char* feature)
{
  while (*defined == ' ') { defined++; }
  while (*feature == ' ') { feature++; }

  while (*defined != '\0' && *feature != '\0')
  {
    if (*defined == '{') {
      placeholder_type ph = placeholder(&defined);
      switch (ph) 
      {
        case PH_STRING: if (string(&feature) == false) return false;
        break; case PH_INT: if (integer(&feature) == false) return false;
        break; case PH_BYTE: if (integer(&feature) == false) return false;
        break; case PH_SHORT: if (integer(&feature) == false) return false;
        break; case PH_DOUBLE: if (double_(&feature) == false) return false;
        break; case PH_FLOAT: if (double_(&feature) == false) return false;
      }
    }
    while (*defined == '\0' && *feature == ' ') 
    {
      feature++;
    }
    while (*defined == ' ' && *feature == '\0') 
    {
      defined++;
    }
    if (*defined == '\0' && *feature == '\0') 
    {
      break; 
    }
    if (*defined != *feature) {
      return false;
    }
    defined++; feature++;
  }
  return true;
}