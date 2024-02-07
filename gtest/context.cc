#include <gtest/gtest.h>

#include "../src/cwt/context.hpp"

class context : public ::testing::Test
{
 protected:
  void TearDown() override { cwt::details::reset_context(); }
};

TEST_F(context, init_string)
{
  cuke::context<std::string>("hello world");
  EXPECT_EQ(cuke::context<std::string>(), std::string("hello world"));
}
TEST_F(context, reset_context)
{
  cuke::context<std::string>("hello world");
  cuke::context<std::size_t>(11);
  EXPECT_EQ(cwt::details::get_context().size(), 2);

  cwt::details::reset_context();
  EXPECT_EQ(cwt::details::get_context().size(), 0);
}

TEST_F(context, init_multiple_objects)
{
struct foo
{
  int value;
};
struct bar
{
  float value;
};
  cuke::context<std::size_t>(11);
  cuke::context<foo>(22);
  cuke::context<bar>(9.9f);

  EXPECT_EQ(cuke::context<std::size_t>(), 11);
  EXPECT_EQ(cuke::context<foo>().value, 22);
  EXPECT_EQ(cuke::context<bar>().value, 9.9f);
}

// TEST_F(context, set_value)
// {
//   cuke::context<std::size_t>(11);
//   cuke::context<foo>(22);
//   cuke::context<bar>(9.9f);

//   cuke::context<bar>().value = 88.8f;

//   EXPECT_EQ(cuke::context<bar>().value, 88.8f);
// }
