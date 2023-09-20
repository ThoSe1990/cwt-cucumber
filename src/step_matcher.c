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
  default: return PH_INVALID;
  }
}

// TODO get never closing bracket!
void advance_after(char c, const char** str)
{
  while(c != **str) /*&& str[pos] != '\0' or '\n' */
  {
    (*str)++;
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

bool is_step(const char* defined, const char* feature)
{
  while (*defined != '\0' && *feature != '\0')
  {
    if (*defined == '{') {
      placeholder_type ph = placeholder(&defined);

      if (ph == PH_STRING) {
        if (*feature != '"') {
          fprintf(stderr, "Expect '\"' for string value.\n");
        }
        feature++;
        advance_after('"', &feature);
      }
    }

    if (*defined != *feature) {
      return false;
    }
    defined++; feature++;
  }
  return true;
}