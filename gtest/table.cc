#include <gtest/gtest.h>

#include "../src/value.hpp"

using namespace cwt::details;

TEST(table, init_table)
{
  table t;
}
TEST(table, init_obj_ptr)
{
  table_ptr t = std::make_unique<table>();
}