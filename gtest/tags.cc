#include <gtest/gtest.h>

extern "C" {
  #include "tag_scanner.h"
  #include "object.h"
}

TEST(tags, tags_1_tag)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("@tag", values);
  ASSERT_EQ(size, 1);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag");
}
TEST(tags, tags_1_not_tag)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("not @tag", values);
  ASSERT_EQ(size, 2);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag");
  EXPECT_STREQ(AS_CSTRING(values[1]), "not");
}
TEST(tags, tags_1_tag_w_paren)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("(@tag)", values);
  ASSERT_EQ(size, 1);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag");
}
TEST(tags, tags_2_tags_and)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("@tag1 and @tag2", values);
  ASSERT_EQ(size, 3);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@tag2");
  EXPECT_STREQ(AS_CSTRING(values[2]), "and");
}
TEST(tags, tags_3_tags_brackets)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("(@tag1 and @tag2) or @tag3", values);
  ASSERT_EQ(size, 5);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@tag2");
  EXPECT_STREQ(AS_CSTRING(values[2]), "and");
  EXPECT_STREQ(AS_CSTRING(values[3]), "@tag3");
  EXPECT_STREQ(AS_CSTRING(values[4]), "or");
}
TEST(tags, tags_3_tags_brackets__2)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("@tag1 and (@tag2 or @tag3)", values);
  ASSERT_EQ(size, 5);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@tag2");
  EXPECT_STREQ(AS_CSTRING(values[2]), "@tag3");
  EXPECT_STREQ(AS_CSTRING(values[3]), "or");
  EXPECT_STREQ(AS_CSTRING(values[4]), "and");
}

TEST(tags, tags_4_tags_1_brackets)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("@tag1 or @tag2 and (@tag3 or @tag4)", values);
  ASSERT_EQ(size, 7);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@tag2");
  EXPECT_STREQ(AS_CSTRING(values[2]), "or");
  EXPECT_STREQ(AS_CSTRING(values[3]), "@tag3");
  EXPECT_STREQ(AS_CSTRING(values[4]), "@tag4");
  EXPECT_STREQ(AS_CSTRING(values[5]), "or");
  EXPECT_STREQ(AS_CSTRING(values[6]), "and");
}

TEST(tags, tags_3_tags_1not_2_brackets)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("not @tag1 or @tag2 and (@tag3 or @tag4)", values);
  ASSERT_EQ(size, 8);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "not");
  EXPECT_STREQ(AS_CSTRING(values[2]), "@tag2");
  EXPECT_STREQ(AS_CSTRING(values[3]), "or");
  EXPECT_STREQ(AS_CSTRING(values[4]), "@tag3");
  EXPECT_STREQ(AS_CSTRING(values[5]), "@tag4");
  EXPECT_STREQ(AS_CSTRING(values[6]), "or");
  EXPECT_STREQ(AS_CSTRING(values[7]), "and");
}

TEST(tags, tags_3_tags_4not_2_brackets)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("not @tag1 or not @tag2 and (not @tag3 or not @tag4)", values);
  ASSERT_EQ(size, 11);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "not");
  EXPECT_STREQ(AS_CSTRING(values[2]), "@tag2");
  EXPECT_STREQ(AS_CSTRING(values[3]), "not");
  EXPECT_STREQ(AS_CSTRING(values[4]), "or");
  EXPECT_STREQ(AS_CSTRING(values[5]), "@tag3");
  EXPECT_STREQ(AS_CSTRING(values[6]), "not");
  EXPECT_STREQ(AS_CSTRING(values[7]), "@tag4");
  EXPECT_STREQ(AS_CSTRING(values[8]), "not");
  EXPECT_STREQ(AS_CSTRING(values[9]), "or");
  EXPECT_STREQ(AS_CSTRING(values[10]), "and");
}

TEST(tags, tags_3_tags_2_brackets)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("@tag1 or ((@tag2 and @tag3) or @tag4)", values);
  ASSERT_EQ(size, 7);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@tag2");
  EXPECT_STREQ(AS_CSTRING(values[2]), "@tag3");
  EXPECT_STREQ(AS_CSTRING(values[3]), "and");
  EXPECT_STREQ(AS_CSTRING(values[4]), "@tag4");
  EXPECT_STREQ(AS_CSTRING(values[5]), "or");
  EXPECT_STREQ(AS_CSTRING(values[6]), "or");
}


TEST(tags, tags_3_tags_3_brackets)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("(@tag1 or ((@tag2 and @tag3) or @tag4)", values);
  ASSERT_EQ(size, 7);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@tag2");
  EXPECT_STREQ(AS_CSTRING(values[2]), "@tag3");
  EXPECT_STREQ(AS_CSTRING(values[3]), "and");
  EXPECT_STREQ(AS_CSTRING(values[4]), "@tag4");
  EXPECT_STREQ(AS_CSTRING(values[5]), "or");
  EXPECT_STREQ(AS_CSTRING(values[6]), "or");
}

TEST(tags, tags_3_tags_x_brackets)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("@tag1 or (((@tag2 and @tag3) or (@tag4 and @tag5) or @tag7) and (@tag8 and @tag9))", values);
  
  ASSERT_EQ(size, 15);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@tag2");
  EXPECT_STREQ(AS_CSTRING(values[2]), "@tag3");
  EXPECT_STREQ(AS_CSTRING(values[3]), "and");
  EXPECT_STREQ(AS_CSTRING(values[4]), "@tag4");
  EXPECT_STREQ(AS_CSTRING(values[5]), "@tag5");
  EXPECT_STREQ(AS_CSTRING(values[6]), "and");
  EXPECT_STREQ(AS_CSTRING(values[7]), "or");
  EXPECT_STREQ(AS_CSTRING(values[8]), "@tag7");
  EXPECT_STREQ(AS_CSTRING(values[9]), "or");
  EXPECT_STREQ(AS_CSTRING(values[10]), "@tag8");
  EXPECT_STREQ(AS_CSTRING(values[11]), "@tag9");
  EXPECT_STREQ(AS_CSTRING(values[12]), "and");
  EXPECT_STREQ(AS_CSTRING(values[13]), "and");
  EXPECT_STREQ(AS_CSTRING(values[14]), "or");
}

TEST(tags, tags_3_tags_x_brackets_2)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("@tag1 or (((@tag2 and @tag3) or (@tag4 and @tag5) and @tag7) and (@tag8 and @tag9))", values);
  
  ASSERT_EQ(size, 15);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@tag2");
  EXPECT_STREQ(AS_CSTRING(values[2]), "@tag3");
  EXPECT_STREQ(AS_CSTRING(values[3]), "and");
  EXPECT_STREQ(AS_CSTRING(values[4]), "@tag4");
  EXPECT_STREQ(AS_CSTRING(values[5]), "@tag5");
  EXPECT_STREQ(AS_CSTRING(values[6]), "and");
  EXPECT_STREQ(AS_CSTRING(values[7]), "or");
  EXPECT_STREQ(AS_CSTRING(values[8]), "@tag7");
  EXPECT_STREQ(AS_CSTRING(values[9]), "and");
  EXPECT_STREQ(AS_CSTRING(values[10]), "@tag8");
  EXPECT_STREQ(AS_CSTRING(values[11]), "@tag9");
  EXPECT_STREQ(AS_CSTRING(values[12]), "and");
  EXPECT_STREQ(AS_CSTRING(values[13]), "and");
  EXPECT_STREQ(AS_CSTRING(values[14]), "or");
}

TEST(tags, some_more_spaces)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("@tag1   and (   @tag2    or@tag3)", values);
  ASSERT_EQ(size, 5);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag1");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@tag2");
  EXPECT_STREQ(AS_CSTRING(values[2]), "@tag3");
  EXPECT_STREQ(AS_CSTRING(values[3]), "or");
  EXPECT_STREQ(AS_CSTRING(values[4]), "and");
}


TEST(tags, syntax_error_1)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("some bad syntax", values);
  ASSERT_EQ(size, 0);
}
TEST(tags, syntax_error_2)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("@tag1 and syntax error", values);
  ASSERT_EQ(size, 0);
}