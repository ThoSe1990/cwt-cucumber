#include <gtest/gtest.h>

#include "../src/context.hpp"

class context : public ::testing::Test
{
 protected:
  void TearDown() override { cuke::internal::reset_context(); }
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
  EXPECT_EQ(cuke::internal::get_context().size(), 2);

  cuke::internal::reset_context();
  EXPECT_EQ(cuke::internal::get_context().size(), 0);
}

struct foo
{
  int value;
};
struct bar
{
  float value;
};
TEST_F(context, init_multiple_objects)
{
  cuke::context<std::size_t>(11);
  cuke::context<foo>(foo{22});
  cuke::context<bar>(bar{9.9F});

  EXPECT_EQ(cuke::context<std::size_t>(), 11);
  EXPECT_EQ(cuke::context<foo>().value, 22);
  EXPECT_EQ(cuke::context<bar>().value, 9.9F);
}

TEST_F(context, set_value)
{
  cuke::context<std::size_t>(11);
  cuke::context<foo>(foo{22});
  cuke::context<bar>(bar{9.9F});

  cuke::context<bar>().value = 88.8F;

  EXPECT_EQ(cuke::context<bar>().value, 88.8F);
  EXPECT_EQ(cuke::context<std::size_t>(), 11);
  EXPECT_EQ(cuke::context<foo>().value, 22);
}
