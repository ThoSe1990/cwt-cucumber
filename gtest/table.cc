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
TEST(table, add_data)
{
  value_array values{value(1), value(std::string("a string")), value(1.123)};
  table t;
  t.m_data = std::move(values);
  EXPECT_EQ(t.m_data.size(), 3);
  EXPECT_EQ(values.size(), 0);
}