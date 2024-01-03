#include <gtest/gtest.h>

#include "../src/cwt/vm.hpp"

using namespace cwt::details;

TEST(vm, init_obj)
{
  vm test_vm;
}
TEST(vm, run_chunk)
{
  vm test_vm;
  test_vm.run("Feature:");
}