#include <gtest/gtest.h>

#include "../src/value.hpp"

using namespace cwt::details;

TEST(table, init_table)
{
  cuke::table t;
}
TEST(table, init_obj_ptr)
{
  table_ptr t = std::make_unique<cuke::table>();
}
// TEST(table, add_data)
// {
//   cuke::value_array values{cuke::value(1), cuke::value(std::string("a string")), cuke::value(1.123)};
//   cuke::table t;
//   t.m_data = std::move(values);
//   EXPECT_EQ(t.m_data.size(), 3);
//   EXPECT_EQ(values.size(), 0);
// }
// TEST(table, value_as_table)
// {
//   cuke::value_array values{cuke::value(1), cuke::value(std::string("a string")), cuke::value(1.123)};
//   cuke::table t;
//   t.m_data = std::move(values);
 
//   cuke::value v(std::move(t));
//   EXPECT_EQ(t.m_data.size(), 0);
//   EXPECT_EQ(v.as<cuke::table>().m_data.size(), 3);
// }