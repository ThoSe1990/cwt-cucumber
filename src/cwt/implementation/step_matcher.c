#include "step_matcher.h"

#include <stdio.h>
#include <string.h>

typedef enum {
  PH_STRING,
  PH_INT,
  PH_INVALID
} placeholder_type;

placeholder_type get_placehoder(const char* begin, const char* end)
{
  switch (*begin)
  {
  case 's': return memcmp("tring", begin+1, end-begin) == 0 ? PH_STRING : PH_INVALID;
  case 'i': return memcmp("nt", begin+1, end-begin) == 0 ? PH_INT : PH_INVALID;
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

void integer(const char** str)
{
  bool dot = false; 
  while (**str != ' ' && **str != '\0' && **str != '\n')
  {
    if (**str == '.') 
    {
      if (dot) 
      {
        fprintf(stderr, "More than one dot in int_value.\n");
      }
      dot = true;
      (*str)++;
    }
    if (is_digit(**str)) 
    {
      (*str)++;
    }
    else 
    {
      fprintf(stderr, "Number contains non digit character.\n");
    }
  }
}

void string(const char** str) 
{
  if (**str != '"') 
  {
    fprintf(stderr, "Expect '\"' for string value.\n");
  }
  (*str)++;
  advance_after('"', str);
}

bool is_step(const char* defined, const char* feature)
{
  while (*defined != '\0' && *feature != '\0')
  {
    if (*defined == '{') {
      placeholder_type ph = placeholder(&defined);
      switch (ph) 
      {
        case PH_STRING: string(&feature);
        break; case PH_INT: integer(&feature);
      }
    }
    if (*defined == '\0' && *feature == '\0') {
      break; 
    }
    if (*defined != *feature) {
      return false;
    }

    defined++; feature++;
  }
  return true;
}