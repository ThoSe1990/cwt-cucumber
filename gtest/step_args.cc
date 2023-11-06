#include <gtest/gtest.h>

extern "C" {
  #include "value.h"
  #include "step_matcher.h"
  #include "memory.h"
  #include "cwt/cucumber.h"
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
    for (int i = 0 ; i < m_args.count ; i++)
    {
      // if (IS_OBJ(m_args.values[i]))
      // {
      //   obj* object = AS_OBJ(m_args.values[i]);
      //   while (object != NULL)
      //   {
      //     obj* next = object->next;
      //     free_object(object);
      //     object = next;
      //   }
      // }
      // obj_string* str = AS_STRING(m_args.values[i]);
      // if (IS_S)
      // std::cout << m_args.values[i].type << std::endl;
      // free(str);
      // if (object)
      //   free_object(object);
      // obj_string* str = (obj_string*)object;
      // if (str)
      //   std::cout << "   still there: " << str->chars << std::endl;
      // if (str->chars)
      // {
      // FREE(obj_string, object);
      //   FREE_ARRAY(char, str->chars, str->length+1);
      // }
    }
    free_value_array(&m_args);
  }

  value_array m_args;
};


TEST_F(step_args, get_values_failing_0)
{
  EXPECT_FALSE(parse_step("this is one step", "this is another", &m_args));
}
TEST_F(step_args, get_values_failing_1)
{
  EXPECT_FALSE(parse_step("never closing {string}", "never closing \"this never closes", &m_args));
}
TEST_F(step_args, get_values_failing_2)
{
  EXPECT_FALSE(parse_step("never closing {string}", "never closing <var1", &m_args));
}
TEST_F(step_args, get_values_failing_3)
{
  EXPECT_FALSE(parse_step("never closing {string}", "never closing 123", &m_args));
}
TEST_F(step_args, get_values_failing_4)
{
  EXPECT_FALSE(parse_step("never closing {int}", "never closing \"this is no int\"", &m_args));
}

TEST_F(step_args, get_values_0)
{
  EXPECT_TRUE(parse_step("step without args", "step without args", &m_args));
  EXPECT_EQ(m_args.count, 0);
}

TEST_F(step_args, get_values_1)
{
  EXPECT_TRUE(parse_step("step with {int}", "step with 99", &m_args));
  ASSERT_EQ(m_args.count, 1);
  EXPECT_EQ(cuke_to_int(&m_args.values[0]), 99);
}

TEST_F(step_args, get_values_2)
{
  EXPECT_TRUE(parse_step("step with {int}", "step with -99", &m_args));
  ASSERT_EQ(m_args.count, 1);
  EXPECT_EQ(cuke_to_int(&m_args.values[0]), -99);
}
TEST_F(step_args, get_values_3)
{
  EXPECT_TRUE(parse_step("step with {double}", "step with 1.123", &m_args));
  ASSERT_EQ(m_args.count, 1);
  EXPECT_EQ(cuke_to_double(&m_args.values[0]), 1.123);
}

TEST_F(step_args, get_values_4)
{
  EXPECT_TRUE(parse_step("step with {float}", "step with -1.123", &m_args));
  ASSERT_EQ(m_args.count, 1);
  EXPECT_EQ(cuke_to_float(&m_args.values[0]), -1.123f);
}
TEST_F(step_args, get_values_5)
{
  EXPECT_TRUE(parse_step("step with {string}", "step with \"a string value\"", &m_args));
  ASSERT_EQ(m_args.count, 1);
  EXPECT_STREQ(cuke_to_string(&m_args.values[0]), "a string value");
  free(AS_CSTRING(m_args.values[0]));
}

TEST_F(step_args, get_values_6)
{
  EXPECT_TRUE(parse_step("arg1 {byte} arg2 {short} arg3 {string} arg4 {int}", "arg1 1 arg2 2 arg3 \"three\" arg4 4", &m_args));
  ASSERT_EQ(m_args.count, 4);
  EXPECT_EQ(cuke_to_byte(&m_args.values[0]), 1);
  EXPECT_EQ(cuke_to_short(&m_args.values[1]), 2);
  EXPECT_STREQ(cuke_to_string(&m_args.values[2]), "three");
  free(AS_CSTRING(m_args.values[2]));
  EXPECT_EQ(cuke_to_int(&m_args.values[3]), 4);
}

TEST_F(step_args, get_values_7)
{
  const char* step = 
R"*(a docstring 
"""
any docstring
"""
)*";
  EXPECT_TRUE(parse_step("a docstring", step, &m_args));
  ASSERT_EQ(m_args.count, 1);
  EXPECT_STREQ(cuke_to_string(&m_args.values[0]), "any docstring");
  free(AS_CSTRING(m_args.values[0]));
}

TEST_F(step_args, get_values_8)
{
  const char* step = 
R"*(some value 123
"""
any docstring
"""
)*";
  EXPECT_TRUE(parse_step("some value {int}", step, &m_args));
  ASSERT_EQ(m_args.count, 2);
  EXPECT_EQ(cuke_to_int(&m_args.values[0]), 123);
  EXPECT_STREQ(cuke_to_string(&m_args.values[1]), "any docstring");
  free(AS_CSTRING(m_args.values[1]));
}