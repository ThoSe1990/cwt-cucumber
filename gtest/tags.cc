#include <gtest/gtest.h>

extern "C" {
  #include "tag_scanner.h"
  #include "object.h"
  #include "memory.h"
}

TEST(tags_rpn, tags_1_tag)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = compile_tag_expression("@tag", values);
  ASSERT_EQ(size, 1);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag");
}
TEST(tags_rpn, tags_1_not_tag)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = compile_tag_expression("not @tag", values);
  ASSERT_EQ(size, 2);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag");
  EXPECT_EQ(AS_LONG(values[1]), TAG_TOKEN_NOT);
}
TEST(tags_rpn, tags_2_not_tag)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = compile_tag_expression("not (@tag1 or @tag2)", values);
  ASSERT_EQ(size, 4);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@tag2");
  EXPECT_EQ(AS_LONG(values[2]), TAG_TOKEN_OR);
  EXPECT_EQ(AS_LONG(values[3]), TAG_TOKEN_NOT);
}
TEST(tags_rpn, tags_xor_tag)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = compile_tag_expression("@tag1 xor @tag2", values);
  ASSERT_EQ(size, 3);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@tag2");
  EXPECT_EQ(AS_LONG(values[2]), TAG_TOKEN_XOR);
}
TEST(tags_rpn, tags_1_tag_w_paren)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = compile_tag_expression("(@tag)", values);
  ASSERT_EQ(size, 1);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag");
}
TEST(tags_rpn, tags_2_tags_and)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = compile_tag_expression("@tag1 and @tag2", values);
  ASSERT_EQ(size, 3);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@tag2");
  EXPECT_EQ(AS_LONG(values[2]), TAG_TOKEN_AND);
}
TEST(tags_rpn, tags_3_tags_brackets)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = compile_tag_expression("(@tag1 and @tag2) or @tag3", values);
  ASSERT_EQ(size, 5);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@tag2");
  EXPECT_EQ(AS_LONG(values[2]), TAG_TOKEN_AND);
  EXPECT_STREQ(AS_CSTRING(values[3]), "@tag3");
  EXPECT_EQ(AS_LONG(values[4]), TAG_TOKEN_OR);
}
TEST(tags_rpn, tags_3_tags_brackets__2)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = compile_tag_expression("@tag1 and (@tag2 or @tag3)", values);
  ASSERT_EQ(size, 5);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@tag2");
  EXPECT_STREQ(AS_CSTRING(values[2]), "@tag3");
  EXPECT_EQ(AS_LONG(values[3]), TAG_TOKEN_OR);
  EXPECT_EQ(AS_LONG(values[4]), TAG_TOKEN_AND);
}

TEST(tags_rpn, tags_4_tags_1_brackets)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = compile_tag_expression("@tag1 or @tag2 and (@tag3 or @tag4)", values);
  ASSERT_EQ(size, 7);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@tag2");
  EXPECT_EQ(AS_LONG(values[2]), TAG_TOKEN_OR);
  EXPECT_STREQ(AS_CSTRING(values[3]), "@tag3");
  EXPECT_STREQ(AS_CSTRING(values[4]), "@tag4");
  EXPECT_EQ(AS_LONG(values[5]), TAG_TOKEN_OR);
  EXPECT_EQ(AS_LONG(values[6]), TAG_TOKEN_AND);
}

TEST(tags_rpn, tags_3_tags_1not_2_brackets)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = compile_tag_expression("not @tag1 or @tag2 and (@tag3 or @tag4)", values);
  ASSERT_EQ(size, 8);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_EQ(AS_LONG(values[1]), TAG_TOKEN_NOT);
  EXPECT_STREQ(AS_CSTRING(values[2]), "@tag2");
  EXPECT_EQ(AS_LONG(values[3]), TAG_TOKEN_OR);
  EXPECT_STREQ(AS_CSTRING(values[4]), "@tag3");
  EXPECT_STREQ(AS_CSTRING(values[5]), "@tag4");
  EXPECT_EQ(AS_LONG(values[6]), TAG_TOKEN_OR);
  EXPECT_EQ(AS_LONG(values[7]), TAG_TOKEN_AND);
}

TEST(tags_rpn, tags_3_tags_4not_2_brackets)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = compile_tag_expression("not @tag1 or not @tag2 and (not @tag3 or not @tag4)", values);
  ASSERT_EQ(size, 11);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_EQ(AS_LONG(values[1]), TAG_TOKEN_NOT);
  EXPECT_STREQ(AS_CSTRING(values[2]), "@tag2");
  EXPECT_EQ(AS_LONG(values[3]), TAG_TOKEN_NOT);
  EXPECT_EQ(AS_LONG(values[4]), TAG_TOKEN_OR);
  EXPECT_STREQ(AS_CSTRING(values[5]), "@tag3");
  EXPECT_EQ(AS_LONG(values[6]), TAG_TOKEN_NOT);
  EXPECT_STREQ(AS_CSTRING(values[7]), "@tag4");
  EXPECT_EQ(AS_LONG(values[8]), TAG_TOKEN_NOT);
  EXPECT_EQ(AS_LONG(values[9]), TAG_TOKEN_OR);
  EXPECT_EQ(AS_LONG(values[10]), TAG_TOKEN_AND);
}

TEST(tags_rpn, tags_3_tags_2_brackets)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = compile_tag_expression("@tag1 or ((@tag2 and @tag3) or @tag4)", values);
  ASSERT_EQ(size, 7);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@tag2");
  EXPECT_STREQ(AS_CSTRING(values[2]), "@tag3");
  EXPECT_EQ(AS_LONG(values[3]), TAG_TOKEN_AND);
  EXPECT_STREQ(AS_CSTRING(values[4]), "@tag4");
  EXPECT_EQ(AS_LONG(values[5]), TAG_TOKEN_OR);
  EXPECT_EQ(AS_LONG(values[6]), TAG_TOKEN_OR);
}


TEST(tags_rpn, tags_3_tags_3_brackets)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = compile_tag_expression("(@tag1 or ((@tag2 and @tag3) or @tag4)", values);
  ASSERT_EQ(size, 7);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@tag2");
  EXPECT_STREQ(AS_CSTRING(values[2]), "@tag3");
  EXPECT_EQ(AS_LONG(values[3]), TAG_TOKEN_AND);
  EXPECT_STREQ(AS_CSTRING(values[4]), "@tag4");
  EXPECT_EQ(AS_LONG(values[5]), TAG_TOKEN_OR);
  EXPECT_EQ(AS_LONG(values[6]), TAG_TOKEN_OR);
}

TEST(tags_rpn, tags_3_tags_x_brackets)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = compile_tag_expression("@tag1 or (((@tag2 and @tag3) or (@tag4 and @tag5) or @tag7) and (@tag8 and @tag9))", values);
  
  ASSERT_EQ(size, 15);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@tag2");
  EXPECT_STREQ(AS_CSTRING(values[2]), "@tag3");
  EXPECT_EQ(AS_LONG(values[3]), TAG_TOKEN_AND);
  EXPECT_STREQ(AS_CSTRING(values[4]), "@tag4");
  EXPECT_STREQ(AS_CSTRING(values[5]), "@tag5");
  EXPECT_EQ(AS_LONG(values[6]), TAG_TOKEN_AND);
  EXPECT_EQ(AS_LONG(values[7]), TAG_TOKEN_OR);
  EXPECT_STREQ(AS_CSTRING(values[8]), "@tag7");
  EXPECT_EQ(AS_LONG(values[9]), TAG_TOKEN_OR);
  EXPECT_STREQ(AS_CSTRING(values[10]), "@tag8");
  EXPECT_STREQ(AS_CSTRING(values[11]), "@tag9");
  EXPECT_EQ(AS_LONG(values[12]), TAG_TOKEN_AND);
  EXPECT_EQ(AS_LONG(values[13]), TAG_TOKEN_AND);
  EXPECT_EQ(AS_LONG(values[14]), TAG_TOKEN_OR);
}

TEST(tags_rpn, tags_3_tags_x_brackets_2)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = compile_tag_expression("@tag1 or (((@tag2 and @tag3) or (@tag4 and @tag5) and @tag7) and (@tag8 and @tag9))", values);
  
  ASSERT_EQ(size, 15);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@tag2");
  EXPECT_STREQ(AS_CSTRING(values[2]), "@tag3");
  EXPECT_EQ(AS_LONG(values[3]), TAG_TOKEN_AND);
  EXPECT_STREQ(AS_CSTRING(values[4]), "@tag4");
  EXPECT_STREQ(AS_CSTRING(values[5]), "@tag5");
  EXPECT_EQ(AS_LONG(values[6]), TAG_TOKEN_AND);
  EXPECT_EQ(AS_LONG(values[7]), TAG_TOKEN_OR);
  EXPECT_STREQ(AS_CSTRING(values[8]), "@tag7");
  EXPECT_EQ(AS_LONG(values[9]), TAG_TOKEN_AND);
  EXPECT_STREQ(AS_CSTRING(values[10]), "@tag8");
  EXPECT_STREQ(AS_CSTRING(values[11]), "@tag9");
  EXPECT_EQ(AS_LONG(values[12]), TAG_TOKEN_AND);
  EXPECT_EQ(AS_LONG(values[13]), TAG_TOKEN_AND);
  EXPECT_EQ(AS_LONG(values[14]), TAG_TOKEN_OR);
}

TEST(tags_rpn, some_more_spaces)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = compile_tag_expression("@tag1   and (   @tag2    or@tag3)", values);
  ASSERT_EQ(size, 5);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@tag2");
  EXPECT_STREQ(AS_CSTRING(values[2]), "@tag3");
  EXPECT_EQ(AS_LONG(values[3]), TAG_TOKEN_OR);
  EXPECT_EQ(AS_LONG(values[4]), TAG_TOKEN_AND);
}


TEST(tags_rpn, syntax_error_1)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = compile_tag_expression("some bad syntax", values);
  ASSERT_EQ(size, 0);
}
TEST(tags_rpn, syntax_error_2)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = compile_tag_expression("@tag1 and syntax error", values);
  ASSERT_EQ(size, 0);
}


#include <string_view>

class tags_evaluation : public ::testing::Test 
{
public: 

  void add_tags(std::string_view first) 
  {
    write_value_array(&m_given_tags, OBJ_VAL(copy_string(first.data(), first.length())));
    // write_c_string(&m_given_tags, first.data(), first.length());
  }
  template <typename... Args>
  void add_tags(std::string_view first, Args... rest) 
  {
    write_value_array(&m_given_tags, OBJ_VAL(copy_string(first.data(), first.length())));
    // write_c_string(&m_given_tags, first.data(), first.length());
    add_tags(rest...); 
  }
  void test_compile_tag_condition(std::string_view str)
  {
    m_size = compile_tag_expression(str.data(), m_rpn_stack);
  }

protected:
  void SetUp() override
  {
    init_value_array(&m_given_tags); 
  }

  void TearDown() override 
  {
    m_size = 0;
    free_value_array(&m_given_tags);
  }

  value_array m_given_tags;
  cuke_value m_rpn_stack[TAGS_RPN_MAX];
  std::size_t m_size; 
};

TEST_F(tags_evaluation, one_tag_true)
{
  add_tags("@hello_world");
  test_compile_tag_condition("@hello_world");
  EXPECT_TRUE(evaluate_tags(m_rpn_stack, m_size, m_given_tags.values, m_given_tags.count));
}
TEST_F(tags_evaluation, one_tag_false)
{
  add_tags("@hello_world");
  test_compile_tag_condition("@no");
  EXPECT_FALSE(evaluate_tags(m_rpn_stack, m_size, m_given_tags.values, m_given_tags.count));
}
TEST_F(tags_evaluation, two_tags_and_true)
{
  add_tags("@tag1");
  add_tags("@tag2");
  test_compile_tag_condition("@tag1 and @tag2");
  EXPECT_TRUE(evaluate_tags(m_rpn_stack, m_size, m_given_tags.values, m_given_tags.count));
}
TEST_F(tags_evaluation, two_tags_and_false)
{
  add_tags("@tag1");
  add_tags("@tag2");
  test_compile_tag_condition("@tag1 and @not_given");
  EXPECT_FALSE(evaluate_tags(m_rpn_stack, m_size, m_given_tags.values, m_given_tags.count));
}
TEST_F(tags_evaluation, _4_tags_true_1)
{
  add_tags("@tag4");
  test_compile_tag_condition("(@tag1 or ((@tag2 and @tag3) or @tag4)");
  EXPECT_TRUE(evaluate_tags(m_rpn_stack, m_size, m_given_tags.values, m_given_tags.count));
}
TEST_F(tags_evaluation, _4_tags_true_2)
{
  add_tags("@tag2");
  add_tags("@tag3");
  test_compile_tag_condition("(@tag1 or ((@tag2 and @tag3) or @tag4)");
  EXPECT_TRUE(evaluate_tags(m_rpn_stack, m_size, m_given_tags.values, m_given_tags.count));
}
TEST_F(tags_evaluation, _4_tags_false)
{
  add_tags("@tag5");
  add_tags("@not_there");
  add_tags("@anything");
  test_compile_tag_condition("(@tag1 or ((@tag2 and @tag3) or @tag4)");
  EXPECT_FALSE(evaluate_tags(m_rpn_stack, m_size, m_given_tags.values, m_given_tags.count));
}
TEST_F(tags_evaluation, empty_condition_not_is_true)
{
  test_compile_tag_condition("not @tag1");
  EXPECT_TRUE(evaluate_tags(m_rpn_stack, m_size, m_given_tags.values, m_given_tags.count));
}
TEST_F(tags_evaluation, a_big_condition_true_1)
{
  add_tags("@tag1");
  add_tags("@tag2");
  test_compile_tag_condition("@tag1 or (((@tag2 and @tag3) or (@tag4 and @tag5) or @tag7) and (@tag8 and @tag9))");
  EXPECT_TRUE(evaluate_tags(m_rpn_stack, m_size, m_given_tags.values, m_given_tags.count));
}
TEST_F(tags_evaluation, a_big_condition_true_2)
{
  add_tags("@tag1");
  add_tags("@tag2");
  add_tags("@tag7");
  add_tags("@tag8");
  add_tags("@tag9");
  test_compile_tag_condition("@tag1 or (((@tag2 and @tag3) or (@tag4 and @tag5) or @tag7) and (@tag8 and @tag9))");
  EXPECT_TRUE(evaluate_tags(m_rpn_stack, m_size, m_given_tags.values, m_given_tags.count));
}
TEST_F(tags_evaluation, a_big_condition_true_3)
{
  add_tags("@tag1");
  add_tags("@tag2");
  add_tags("@tag7");
  add_tags("@tag8");
  add_tags("@tag9");
  test_compile_tag_condition("@tag1 or (((@tag2 and @tag3) or (@tag4 and @tag5) or @tag7) and (@tag8 and @tag9))");
  EXPECT_TRUE(evaluate_tags(m_rpn_stack, m_size, m_given_tags.values, m_given_tags.count));
}
TEST_F(tags_evaluation, a_big_condition_true_w_xor_4)
{
  add_tags("@tag4");
  add_tags("@tag5");
  add_tags("@tag8");
  add_tags("@tag9");
  test_compile_tag_condition("@tag1 xor (((@tag2 and @tag3) or (@tag4 and @tag5) or @tag7) and (@tag8 and @tag9))");
  EXPECT_TRUE(evaluate_tags(m_rpn_stack, m_size, m_given_tags.values, m_given_tags.count));
}
TEST_F(tags_evaluation, a_big_condition_false_w_xor)
{
  add_tags("@tag1");
  add_tags("@tag4");
  add_tags("@tag5");
  add_tags("@tag8");
  add_tags("@tag9");
  test_compile_tag_condition("@tag1 xor (((@tag2 and @tag3) or (@tag4 and @tag5) or @tag7) and (@tag8 and @tag9))");
  EXPECT_FALSE(evaluate_tags(m_rpn_stack, m_size, m_given_tags.values, m_given_tags.count));
}
TEST_F(tags_evaluation, a_big_condition_false_1)
{
  add_tags("@tag2");
  add_tags("@tag5");
  add_tags("@tag8");
  add_tags("@tag9");
  test_compile_tag_condition("@tag1 or (((@tag2 and @tag3) or (@tag4 and @tag5) or @tag7) and (@tag8 and @tag9))");
  EXPECT_FALSE(evaluate_tags(m_rpn_stack, m_size, m_given_tags.values, m_given_tags.count));
}
TEST_F(tags_evaluation, a_big_condition_false_2)
{
  test_compile_tag_condition("@tag1 or (((@tag2 and @tag3) or (@tag4 and @tag5) or @tag7) and (@tag8 and @tag9))");
  EXPECT_FALSE(evaluate_tags(m_rpn_stack, m_size, m_given_tags.values, m_given_tags.count));
}
TEST_F(tags_evaluation, xor_true)
{
  add_tags("@tag2");
  test_compile_tag_condition("@tag1 xor @tag2");
  EXPECT_TRUE(evaluate_tags(m_rpn_stack, m_size, m_given_tags.values, m_given_tags.count));
}
TEST_F(tags_evaluation, syntax_error_1)
{
  add_tags("@tag2");
  test_compile_tag_condition("@tag1 helloworld @tag2");
  EXPECT_FALSE(evaluate_tags(m_rpn_stack, m_size, m_given_tags.values, m_given_tags.count));
}
TEST_F(tags_evaluation, syntax_error_2)
{
  add_tags("@tag2");
  test_compile_tag_condition("tag1 @tag2");
  EXPECT_FALSE(evaluate_tags(m_rpn_stack, m_size, m_given_tags.values, m_given_tags.count));
}

TEST_F(tags_evaluation, syntax_error_3)
{
  add_tags("@tag");
  test_compile_tag_condition("$tag");
  EXPECT_FALSE(evaluate_tags(m_rpn_stack, m_size, m_given_tags.values, m_given_tags.count));
}
TEST_F(tags_evaluation, weird_tags_1)
{
  add_tags("@ta!!!!g");
  test_compile_tag_condition("@ta!!!!g or @tag2");
  EXPECT_TRUE(evaluate_tags(m_rpn_stack, m_size, m_given_tags.values, m_given_tags.count));
}
TEST_F(tags_evaluation, weird_tags_2)
{
  add_tags("@ta#$%^^&*(*(!!g");
  test_compile_tag_condition("@ta#$%^^&*(*(!!g or @tag2");
  EXPECT_TRUE(evaluate_tags(m_rpn_stack, m_size, m_given_tags.values, m_given_tags.count));
}