#ifndef cwt_cucumber_common_h
#define cwt_cucumber_common_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#ifdef STACK_TRACE
  #define DEBUG_PRINT_CODE
  #define DEBUG_TRACE_EXTENSION
#endif

#define UINT16_COUNT (UINT16_MAX + 1)

#endif