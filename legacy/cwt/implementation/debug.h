#ifndef cwt_cucumber_debug_h
#define cwt_cucumber_debug_h

#include "chunk.h"

void disassemble_chunk(chunk* c, const char* name);
int disassemble_instruction(chunk* c, int offset);

#endif 