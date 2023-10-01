#ifndef cwt_cucumber_step_matcher_h
#define cwt_cucumber_step_matcher_h

#include "common.h"
#include "value.h"

bool parse_step(const char* defined, const char* feature, value_array* args);

#endif