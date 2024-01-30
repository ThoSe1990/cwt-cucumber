#include <gtest/gtest.h>

#include "../src/cwt/tags.hpp"

using namespace cwt::details;

TEST(tag_compiler, tags_1_tag)
{

  tag_compiler tc("@tag");
  ASSERT_EQ(tc.size(), 1);
  EXPECT_EQ(tc[0].value, "@tag");
}
TEST(tag_compiler, tags_1_not_tag)
{

  tag_compiler tc("not @tag");
  ASSERT_EQ(tc.size(), 2);
  EXPECT_EQ(tc[0].value, "@tag");
  EXPECT_EQ(tc[1].token, token_type::_not);
}
TEST(tag_compiler, tags_2_not_tag)
{

  tag_compiler tc("not (@tag1 or @tag2)");
  ASSERT_EQ(tc.size(), 4);
  EXPECT_EQ(tc[0].value, "@tag1");
  EXPECT_EQ(tc[1].value, "@tag2");
  EXPECT_EQ(tc[2].token, token_type::_or);
  EXPECT_EQ(tc[3].token, token_type::_not);
}
TEST(tag_compiler, tags_xor_tag)
{

  tag_compiler tc("@tag1 xor @tag2");
  ASSERT_EQ(tc.size(), 3);
  EXPECT_EQ(tc[0].value, "@tag1");
  EXPECT_EQ(tc[1].value, "@tag2");
  EXPECT_EQ(tc[2].token, token_type::_xor);
}
TEST(tag_compiler, tags_1_tag_w_paren)
{

  tag_compiler tc("(@tag)");
  ASSERT_EQ(tc.size(), 1);
  EXPECT_EQ(tc[0].value, "@tag");
}
TEST(tag_compiler, tags_2_tags_and)
{

  tag_compiler tc("@tag1 and @tag2");
  ASSERT_EQ(tc.size(), 3);
  EXPECT_EQ(tc[0].value, "@tag1");
  EXPECT_EQ(tc[1].value, "@tag2");
  EXPECT_EQ(tc[2].token, token_type::_and);
}
TEST(tag_compiler, tags_3_tags_brackets)
{

  tag_compiler tc("(@tag1 and @tag2) or @tag3");
  ASSERT_EQ(tc.size(), 5);
  EXPECT_EQ(tc[0].value, "@tag1");
  EXPECT_EQ(tc[1].value, "@tag2");
  EXPECT_EQ(tc[2].token, token_type::_and);
  EXPECT_EQ(tc[3].value, "@tag3");
  EXPECT_EQ(tc[4].token, token_type::_or);
}
TEST(tag_compiler, tags_3_tags_brackets__2)
{

  tag_compiler tc("@tag1 and (@tag2 or @tag3)");
  ASSERT_EQ(tc.size(), 5);
  EXPECT_EQ(tc[0].value, "@tag1");
  EXPECT_EQ(tc[1].value, "@tag2");
  EXPECT_EQ(tc[2].value, "@tag3");
  EXPECT_EQ(tc[3].token, token_type::_or);
  EXPECT_EQ(tc[4].token, token_type::_and);
}

TEST(tag_compiler, tags_4_tags_1_brackets)
{

  tag_compiler tc("@tag1 or @tag2 and (@tag3 or @tag4)");
  ASSERT_EQ(tc.size(), 7);
  EXPECT_EQ(tc[0].value, "@tag1");
  EXPECT_EQ(tc[1].value, "@tag2");
  EXPECT_EQ(tc[2].token, token_type::_or);
  EXPECT_EQ(tc[3].value, "@tag3");
  EXPECT_EQ(tc[4].value, "@tag4");
  EXPECT_EQ(tc[5].token, token_type::_or);
  EXPECT_EQ(tc[6].token, token_type::_and);
}

TEST(tag_compiler, tags_3_tags_1not_2_brackets)
{

  tag_compiler tc("not @tag1 or @tag2 and (@tag3 or @tag4)");
  ASSERT_EQ(tc.size(), 8);
  EXPECT_EQ(tc[0].value, "@tag1");
  EXPECT_EQ(tc[1].token, token_type::_not);
  EXPECT_EQ(tc[2].value, "@tag2");
  EXPECT_EQ(tc[3].token, token_type::_or);
  EXPECT_EQ(tc[4].value, "@tag3");
  EXPECT_EQ(tc[5].value, "@tag4");
  EXPECT_EQ(tc[6].token, token_type::_or);
  EXPECT_EQ(tc[7].token, token_type::_and);
}

TEST(tag_compiler, tags_3_tags_4not_2_brackets)
{

  tag_compiler tc("not @tag1 or not @tag2 and (not @tag3 or not @tag4)");
  ASSERT_EQ(tc.size(), 11);
  EXPECT_EQ(tc[0].value, "@tag1");
  EXPECT_EQ(tc[1].token, token_type::_not);
  EXPECT_EQ(tc[2].value, "@tag2");
  EXPECT_EQ(tc[3].token, token_type::_not);
  EXPECT_EQ(tc[4].token, token_type::_or);
  EXPECT_EQ(tc[5].value, "@tag3");
  EXPECT_EQ(tc[6].token, token_type::_not);
  EXPECT_EQ(tc[7].value, "@tag4");
  EXPECT_EQ(tc[8].token, token_type::_not);
  EXPECT_EQ(tc[9].token, token_type::_or);
  EXPECT_EQ(tc[10].token, token_type::_and);
}

TEST(tag_compiler, tags_3_tags_2_brackets)
{

  tag_compiler tc("@tag1 or ((@tag2 and @tag3) or @tag4)");
  ASSERT_EQ(tc.size(), 7);
  EXPECT_EQ(tc[0].value, "@tag1");
  EXPECT_EQ(tc[1].value, "@tag2");
  EXPECT_EQ(tc[2].value, "@tag3");
  EXPECT_EQ(tc[3].token, token_type::_and);
  EXPECT_EQ(tc[4].value, "@tag4");
  EXPECT_EQ(tc[5].token, token_type::_or);
  EXPECT_EQ(tc[6].token, token_type::_or);
}


TEST(tag_compiler, tags_3_tags_3_brackets)
{

  tag_compiler tc("(@tag1 or ((@tag2 and @tag3) or @tag4)");
  ASSERT_EQ(tc.size(), 7);
  EXPECT_EQ(tc[0].value, "@tag1");
  EXPECT_EQ(tc[1].value, "@tag2");
  EXPECT_EQ(tc[2].value, "@tag3");
  EXPECT_EQ(tc[3].token, token_type::_and);
  EXPECT_EQ(tc[4].value, "@tag4");
  EXPECT_EQ(tc[5].token, token_type::_or);
  EXPECT_EQ(tc[6].token, token_type::_or);
}

TEST(tag_compiler, tags_3_tags_x_brackets)
{

  tag_compiler tc("@tag1 or (((@tag2 and @tag3) or (@tag4 and @tag5) or @tag7) and (@tag8 and @tag9))");
  
  ASSERT_EQ(tc.size(), 15);
  EXPECT_EQ(tc[0].value, "@tag1");
  EXPECT_EQ(tc[1].value, "@tag2");
  EXPECT_EQ(tc[2].value, "@tag3");
  EXPECT_EQ(tc[3].token, token_type::_and);
  EXPECT_EQ(tc[4].value, "@tag4");
  EXPECT_EQ(tc[5].value, "@tag5");
  EXPECT_EQ(tc[6].token, token_type::_and);
  EXPECT_EQ(tc[7].token, token_type::_or);
  EXPECT_EQ(tc[8].value, "@tag7");
  EXPECT_EQ(tc[9].token, token_type::_or);
  EXPECT_EQ(tc[10].value, "@tag8");
  EXPECT_EQ(tc[11].value, "@tag9");
  EXPECT_EQ(tc[12].token, token_type::_and);
  EXPECT_EQ(tc[13].token, token_type::_and);
  EXPECT_EQ(tc[14].token, token_type::_or);
}

TEST(tag_compiler, tags_3_tags_x_brackets_2)
{

  tag_compiler tc("@tag1 or (((@tag2 and @tag3) or (@tag4 and @tag5) and @tag7) and (@tag8 and @tag9))");
  
  ASSERT_EQ(tc.size(), 15);
  EXPECT_EQ(tc[0].value, "@tag1");
  EXPECT_EQ(tc[1].value, "@tag2");
  EXPECT_EQ(tc[2].value, "@tag3");
  EXPECT_EQ(tc[3].token, token_type::_and);
  EXPECT_EQ(tc[4].value, "@tag4");
  EXPECT_EQ(tc[5].value, "@tag5");
  EXPECT_EQ(tc[6].token, token_type::_and);
  EXPECT_EQ(tc[7].token, token_type::_or);
  EXPECT_EQ(tc[8].value, "@tag7");
  EXPECT_EQ(tc[9].token, token_type::_and);
  EXPECT_EQ(tc[10].value, "@tag8");
  EXPECT_EQ(tc[11].value, "@tag9");
  EXPECT_EQ(tc[12].token, token_type::_and);
  EXPECT_EQ(tc[13].token, token_type::_and);
  EXPECT_EQ(tc[14].token, token_type::_or);
}

TEST(tag_compiler, some_more_spaces)
{

  tag_compiler tc("@tag1   and (   @tag2    or@tag3)");
  ASSERT_EQ(tc.size(), 5);
  EXPECT_EQ(tc[0].value, "@tag1");
  EXPECT_EQ(tc[1].value, "@tag2");
  EXPECT_EQ(tc[2].value, "@tag3");
  EXPECT_EQ(tc[3].token, token_type::_or);
  EXPECT_EQ(tc[4].token, token_type::_and);
}


TEST(tag_compiler, syntax_error_1)
{

  tag_compiler tc("some bad syntax");
  ASSERT_EQ(tc.size(), 0);
}
TEST(tag_compiler, syntax_error_2)
{

  tag_compiler tc("@tag1 and syntax error");
  ASSERT_EQ(tc.size(), 0);
}
