#include <gtest/gtest.h>

#include "../src/cwt/util.hpp"

using namespace cwt::details;


TEST(util_filepath, absolute_path_win_wo_line)
{
  const std::string str = "C:\\git\\cwt-cucumber\\any.feature";
  auto [fp, lines] = filepath_and_lines(str);

  EXPECT_EQ(fp, std::string("C:\\git\\cwt-cucumber\\any.feature"));
  EXPECT_EQ(lines.size(), 0);
}
TEST(util_filepath, relative_path_win_wo_line_1)
{
  const std::string str = "cwt-cucumber\\any.feature";
  auto [fp, lines] = filepath_and_lines(str);

  EXPECT_EQ(fp, std::string("cwt-cucumber\\any.feature"));
  EXPECT_EQ(lines.size(), 0);
}
TEST(util_filepath, relative_path_win_wo_line_2)
{
  const std::string str = ".\\cwt-cucumber\\any.feature";
  auto [fp, lines] = filepath_and_lines(str);

  EXPECT_EQ(fp, std::string(".\\cwt-cucumber\\any.feature"));
  EXPECT_EQ(lines.size(), 0);
}
TEST(util_filepath, relative_path_unix_wo_line_1)
{
  const std::string str = "cwt-cucumber/any.feature";
  auto [fp, lines] = filepath_and_lines(str);

  EXPECT_EQ(fp, std::string("cwt-cucumber/any.feature"));
  EXPECT_EQ(lines.size(), 0);
}
TEST(util_filepath, relative_path_unix_wo_line_2)
{
  const std::string str = "./cwt-cucumber/any.feature";
  auto [fp, lines] = filepath_and_lines(str);

  EXPECT_EQ(fp, std::string("./cwt-cucumber/any.feature"));
  EXPECT_EQ(lines.size(), 0);
}
TEST(util_filepath, absolute_path_unix_wo_line)
{
  const std::string str = "/c/git/cwt-cucumber/any.feature";
  auto [fp, lines] = filepath_and_lines(str);

  EXPECT_EQ(fp, std::string("/c/git/cwt-cucumber/any.feature"));
  EXPECT_EQ(lines.size(), 0);
}
TEST(util_filepath, absolute_path_win_single_line)
{
  const std::string str = "C:\\git\\cwt-cucumber\\any.feature:12";
  auto [fp, lines] = filepath_and_lines(str);

  EXPECT_EQ(fp, std::string("C:\\git\\cwt-cucumber\\any.feature"));
  EXPECT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0], 12);
}
TEST(util_filepath, relative_path_win_single_line_1)
{
  const std::string str = "some_dir\\any.feature:12";
  auto [fp, lines] = filepath_and_lines(str);

  EXPECT_EQ(fp, std::string("some_dir\\any.feature"));
  EXPECT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0], 12);
}
TEST(util_filepath, relative_path_win_single_line_2)
{
  const std::string str = ".\\some_dir\\any.feature:12";
  auto [fp, lines] = filepath_and_lines(str);

  EXPECT_EQ(fp, std::string(".\\some_dir\\any.feature"));
  EXPECT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0], 12);
}
TEST(util_filepath, absolute_path_win_multi_line)
{
  const std::string str = "C:\\git\\cwt-cucumber\\any.feature:12:123:999:123456789";
  auto [fp, lines] = filepath_and_lines(str);

  EXPECT_EQ(fp, std::string("C:\\git\\cwt-cucumber\\any.feature"));
  EXPECT_EQ(lines.size(), 4);
  EXPECT_EQ(lines[3], 12);
  EXPECT_EQ(lines[2], 123);
  EXPECT_EQ(lines[1], 999);
  EXPECT_EQ(lines[0], 123456789);
}
TEST(util_filepath, relative_path_win_multi_line_1)
{
  const std::string str = "some_dir\\any.feature:12:123:999:123456789";
  auto [fp, lines] = filepath_and_lines(str);

  EXPECT_EQ(fp, std::string("some_dir\\any.feature"));
  EXPECT_EQ(lines.size(), 4);
  EXPECT_EQ(lines[3], 12);
  EXPECT_EQ(lines[2], 123);
  EXPECT_EQ(lines[1], 999);
  EXPECT_EQ(lines[0], 123456789);
}
TEST(util_filepath, relative_path_win_multi_line_2)
{
  const std::string str = ".\\some_dir\\any.feature:12:123:999:123456789";
  auto [fp, lines] = filepath_and_lines(str);

  EXPECT_EQ(fp, std::string(".\\some_dir\\any.feature"));
  EXPECT_EQ(lines.size(), 4);
  EXPECT_EQ(lines[3], 12);
  EXPECT_EQ(lines[2], 123);
  EXPECT_EQ(lines[1], 999);
  EXPECT_EQ(lines[0], 123456789);
}
TEST(util_filepath, absolute_path_unix_single_line)
{
  const std::string str = "/c/git/cwt-cucumber/any.feature:12";
  auto [fp, lines] = filepath_and_lines(str);

  EXPECT_EQ(fp, std::string("/c/git/cwt-cucumber/any.feature"));
  EXPECT_EQ(lines.size(), 1);
  EXPECT_EQ(lines[0], 12);
}
TEST(util_filepath, absolute_path_unix_multi_line)
{
  const std::string str = "/c/git/cwt-cucumber/any.feature:12:123:999:123456789";
  auto [fp, lines] = filepath_and_lines(str);

  EXPECT_EQ(fp, std::string("/c/git/cwt-cucumber/any.feature"));
  EXPECT_EQ(lines.size(), 4);
  EXPECT_EQ(lines[3], 12);
  EXPECT_EQ(lines[2], 123);
  EXPECT_EQ(lines[1], 999);
  EXPECT_EQ(lines[0], 123456789);
}