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
TEST(tags, tags_2_tags_and)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("@tag and @another", values);
  ASSERT_EQ(size, 3);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@another");
  EXPECT_STREQ(AS_CSTRING(values[2]), "and");
}
TEST(tags, tags_3_tags_brackets)
{
  cuke_value values[TAGS_RPN_MAX];
  int size = tags_to_rpn_stack("(@tag and @another) or @this", values);
  ASSERT_EQ(size, 5);
  EXPECT_STREQ(AS_CSTRING(values[0]), "@tag");
  EXPECT_STREQ(AS_CSTRING(values[1]), "@another");
  EXPECT_STREQ(AS_CSTRING(values[2]), "and");
  EXPECT_STREQ(AS_CSTRING(values[3]), "@this");
  EXPECT_STREQ(AS_CSTRING(values[4]), "or");
}