#ifndef cwt_cucumber_scanner_h
#define cwt_cucumber_scanner_h

typedef enum 
{
  TOKEN_VERTICAL,
  TOKEN_MINUS,
  TOKEN_TRIPPLE_QUOTES,
  
  TOKEN_LANGUAGE, 
  TOKEN_FEATURE, 
  TOKEN_BACKGROUND,
  TOKEN_SCENARIO,
  TOKEN_SCENARIO_OUTLINE,
  TOKEN_STEP,
  TOKEN_EXAMPLES,

  TOKEN_VAR,
  TOKEN_TAG,
  TOKEN_STRING,
  TOKEN_DOC_STRING,
  TOKEN_LONG,
  TOKEN_DOUBLE,

  TOKEN_VARIABLE,
  TOKEN_TEXT,  
  TOKEN_LINEBREAK, 

  TOKEN_ERROR, TOKEN_EOF, 
  NO_TOKEN, CHECK_NEXT
} token_type;

typedef struct {
  token_type type;
  const char* start;
  int length;
  int line;
} token;

void init_scanner(const char* source, const char* filename);
token scan_token(); 
const char* filename();

#endif