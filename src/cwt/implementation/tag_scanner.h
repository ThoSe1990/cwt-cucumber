#ifndef cwt_cucumber_tag_scanner_h
#define cwt_cucumber_tag_scanner_h

typedef enum 
{
  TAG_TOKEN_TAG,
  TAG_TOKEN_AND,
  TAG_TOKEN_OR,
  TAG_TOKEN_NOT,
  TAG_TOKEN_LEFT_PAREN,
  TAG_TOKEN_RIGHT_PAREN,
  TAG_TOKEN_ERROR,
  TAG_TOKEN_EOL
} tag_token_type;

typedef struct {
  tag_token_type type;
  const char* start;
  int length;
} tag_token;

void init_tag_scanner(const char* source);
tag_token scan_tag_token(); 

#endif