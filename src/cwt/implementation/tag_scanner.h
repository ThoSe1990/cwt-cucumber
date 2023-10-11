#ifndef cwt_cucumber_tag_scanner_h
#define cwt_cucumber_tag_scanner_h

#include "../cucumber_value.h"

#define TAGS_RPN_MAX 32

int tags_to_rpn_stack(const char* tags, cuke_value* result);

#endif