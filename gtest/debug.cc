#include <gtest/gtest.h>

#define PRINT_STACK 1

#include "../src/chunk.hpp"
#include "../src/debug.hpp"

using namespace cuke::internal;

TEST(debug, print_chunk)
{
  chunk c; 
  c.push_byte(op_code::constant, 1);
  c.push_byte(c.make_constant(std::string("string_value")), 1);
  c.push_byte(op_code::constant, 2);
  c.push_byte(c.make_constant(123), 2);
  c.push_byte(op_code::constant, 2);
  c.push_byte(c.make_constant(123.123), 2);
  disassemble_chunk(c, "test_chunk");
}

#undef PRINT_STACK