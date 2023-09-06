#ifndef cwt_cucumber_scanner_h
#define cwt_cucumber_scanner_h

typedef enum 
{
  TOKEN_POUND,

  TOKEN_LANGUAGE,
  TOKEN_FEATURE,
  TOKEN_VERTICAL,
 
  TOKEN_SCENARIO,
  TOKEN_GIVEN, TOKEN_WHEN, TOKEN_THEN,
  TOKEN_STRING,

  TOKEN_STEP,
  TOKEN_LINEBREAK,
  TOKEN_ERROR, TOKEN_EOF, NO_TOKEN
} token_type;

typedef struct {
  token_type type;
  const char* start;
  int length;
  int line;
} token;

void init_scanner(const char* source);
token scan_token(bool ignore_keywords); 

#endif