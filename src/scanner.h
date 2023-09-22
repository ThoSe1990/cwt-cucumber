#ifndef cwt_cucumber_scanner_h
#define cwt_cucumber_scanner_h

typedef enum 
{
  TOKEN_COLON, 
  TOKEN_VERTICAL,
  TOKEN_DOC_STRING,

  TOKEN_LANGUAGE, 
  TOKEN_FEATURE, 
  TOKEN_SCENARIO,
  TOKEN_STEP,
  TOKEN_EXAMPLES,

  TOKEN_STRING,
  TOKEN_INT,

  TOKEN_VARIABLE,
  TOKEN_TEXT,  
  TOKEN_LINEBREAK, 

  TOKEN_ERROR, TOKEN_EOF, 
  NO_TOKEN
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