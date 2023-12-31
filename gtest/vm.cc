#include <gtest/gtest.h>

#include "../src/cwt/vm.hpp"

using namespace cwt::details;

chunk create_print_chunk()
{
  chunk c;
  c.push_byte(op_code::constant, 1);
  c.push_byte(1, 1);
  c.push_byte(op_code::constant, 2);
  c.push_byte(2, 2);
  c.push_byte(op_code::constant, 3);
  c.push_byte(3, 3);
  return c;
}

TEST(vm, init_obj)
{
  vm test_vm;
}
TEST(vm, run_chunk)
{
  vm test_vm;
  test_vm.run(create_print_chunk());
}