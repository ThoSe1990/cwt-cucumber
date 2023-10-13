#ifndef cwt_cucumber_tag_scanner_h
#define cwt_cucumber_tag_scanner_h

#include "value.h"

#define TAGS_RPN_MAX 32

typedef enum 
{
  TAG_TOKEN_ERROR,
  TAG_TOKEN_TAG,
  TAG_TOKEN_AND,
  TAG_TOKEN_OR,
  TAG_TOKEN_XOR,
  TAG_TOKEN_NOT,
  TAG_TOKEN_LEFT_PAREN,
  TAG_TOKEN_RIGHT_PAREN,
  TAG_TOKEN_EOL
} tag_token_type;

int compile_tag_expression(const char* tags, cuke_value* result);
bool evaluate_tags(cuke_value* rpn_stack, int rpn_size, value_array* tags);

// TODO : 
void test_push (value_array* arr, const char* c);

#endif