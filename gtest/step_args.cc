#include <gtest/gtest.h>

extern "C" {
  #include "value.h"
  #include "step_matcher.h"
  #include "cucumber.h"
}


class step_args : public ::testing::Test 
{
protected:
  void SetUp() override
  {
    init_value_array(&m_args); 
  }

  void TearDown() override 
  {
    free_value_array(&m_args);
  }

  value_array m_args;
};


TEST_F(step_args, get_values_0)
{
  EXPECT_TRUE(parse_step("step without args", "step without args", &m_args));
  EXPECT_EQ(m_args.count, 0);
}

TEST_F(step_args, get_values_1)
{
  EXPECT_TRUE(parse_step("step with {int}", "step with 99", &m_args));
  ASSERT_EQ(m_args.count, 1);
  EXPECT_EQ(cwtc_to_int(&m_args.values[0]), 99);
}

TEST_F(step_args, get_values_2)
{
  EXPECT_TRUE(parse_step("step with {int}", "step with -99", &m_args));
  ASSERT_EQ(m_args.count, 1);
  EXPECT_EQ(cwtc_to_int(&m_args.values[0]), -99);
}
TEST_F(step_args, get_values_3)
{
  EXPECT_TRUE(parse_step("step with {double}", "step with 1.123", &m_args));
  ASSERT_EQ(m_args.count, 1);
  EXPECT_EQ(cwtc_to_double(&m_args.values[0]), 1.123);
}

TEST_F(step_args, get_values_4)
{
  EXPECT_TRUE(parse_step("step with {float}", "step with -1.123", &m_args));
  ASSERT_EQ(m_args.count, 1);
  EXPECT_EQ(cwtc_to_float(&m_args.values[0]), -1.123f);
}
TEST_F(step_args, get_values_5)
{
  EXPECT_TRUE(parse_step("step with {string}", "step with \"a string value\"", &m_args));
  ASSERT_EQ(m_args.count, 1);
  EXPECT_STREQ(cwtc_to_string(&m_args.values[0]), "a string value");
}

TEST_F(step_args, get_values_6)
{
  EXPECT_TRUE(parse_step("arg1 {byte} arg2 {short} arg3 {string} arg4 {int}", "arg1 1 arg2 2 arg3 \"three\" arg4 4", &m_args));
  ASSERT_EQ(m_args.count, 4);
  EXPECT_EQ(cwtc_to_byte(&m_args.values[0]), 1);
  EXPECT_EQ(cwtc_to_short(&m_args.values[1]), 2);
  EXPECT_STREQ(cwtc_to_string(&m_args.values[2]), "three");
  EXPECT_EQ(cwtc_to_int(&m_args.values[3]), 4);
}
