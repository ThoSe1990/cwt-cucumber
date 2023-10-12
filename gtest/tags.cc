#include <gtest/gtest.h>

extern "C" {
  #include "tag_scanner.h"
  #include "object.h"
}

TEST(tags_rpn, tags_1_tag)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("@tag", values);
  ASSERT_EQ(size, 1);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag");
}
TEST(tags_rpn, tags_1_not_tag)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("not @tag", values);
  ASSERT_EQ(size, 2);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag");
  EXPECT_EQ(AS_LONG(values[1]), TAG_TOKEN_NOT);
}
TEST(tags_rpn, tags_2_not_tag)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("not (@tag1 or @tag2)", values);
  ASSERT_EQ(size, 4);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@tag2");
  EXPECT_EQ(AS_LONG(values[2]), TAG_TOKEN_OR);
  EXPECT_EQ(AS_LONG(values[3]), TAG_TOKEN_NOT);
}
TEST(tags_rpn, tags_1_tag_w_paren)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("(@tag)", values);
  ASSERT_EQ(size, 1);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag");
}
TEST(tags_rpn, tags_2_tags_and)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("@tag1 and @tag2", values);
  ASSERT_EQ(size, 3);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@tag2");
  EXPECT_EQ(AS_LONG(values[2]), TAG_TOKEN_AND);
}
TEST(tags_rpn, tags_3_tags_brackets)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("(@tag1 and @tag2) or @tag3", values);
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
  int size = tags_to_rpn_stack("@tag1 and (@tag2 or @tag3)", values);
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
  int size = tags_to_rpn_stack("@tag1 or @tag2 and (@tag3 or @tag4)", values);
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
  int size = tags_to_rpn_stack("not @tag1 or @tag2 and (@tag3 or @tag4)", values);
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
  int size = tags_to_rpn_stack("not @tag1 or not @tag2 and (not @tag3 or not @tag4)", values);
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
  int size = tags_to_rpn_stack("@tag1 or ((@tag2 and @tag3) or @tag4)", values);
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
  int size = tags_to_rpn_stack("(@tag1 or ((@tag2 and @tag3) or @tag4)", values);
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
  int size = tags_to_rpn_stack("@tag1 or (((@tag2 and @tag3) or (@tag4 and @tag5) or @tag7) and (@tag8 and @tag9))", values);
  
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
  int size = tags_to_rpn_stack("@tag1 or (((@tag2 and @tag3) or (@tag4 and @tag5) and @tag7) and (@tag8 and @tag9))", values);
  
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
  int size = tags_to_rpn_stack("@tag1   and (   @tag2    or@tag3)", values);
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
  int size = tags_to_rpn_stack("some bad syntax", values);
  ASSERT_EQ(size, 0);
}
TEST(tags_rpn, syntax_error_2)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("@tag1 and syntax error", values);
  ASSERT_EQ(size, 0);
}




TEST(tags_evaluation, one_tag)
{
  cuke_value rpn[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("@tag", rpn);

  value_array tags;
  init_value_array(&tags);

  test_push(&tags, "@tag");
  
  EXPECT_TRUE(evaluate_tags(rpn, size, &tags));
  free_value_array(&tags);
}