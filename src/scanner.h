#ifndef cwt_cucumber_scanner_h
#define cwt_cucumber_scanner_h

typedef enum 
{
  TOKEN_COLON, // 0
  TOKEN_VERTICAL, // 1

  TOKEN_LANGUAGE, // 2
  TOKEN_FEATURE, // 3
  TOKEN_SCENARIO, // 4
  TOKEN_STEP, // 5

  TOKEN_STRING, // 6
  TOKEN_NUMBER, // 7

  TOKEN_TEXT, // 8 
  TOKEN_LINEBREAK, // 9

  TOKEN_ERROR, TOKEN_EOF, 
  NO_TOKEN
} token_type;

typedef struct {
  token_type type;
  const char* start;
  int length;
  int line;
} token;

void init_scanner(const char* source);
token scan_token(); 
token scan_line();
token scan_identifier_or_line(token_type t);

#endif