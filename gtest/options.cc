#include <format>
#include <gtest/gtest.h>

#include "../src/options.hpp"
#include "paths.hpp"

TEST(options, init_obj)
{
  cuke::internal::terminal_arguments targs;
  EXPECT_FALSE(targs.get_options().quiet);
  EXPECT_TRUE(targs.get_options().files.empty());
  EXPECT_TRUE(targs.get_options().tags.empty());
}
TEST(options, file_path_doesnt_exist)
{
  const char* argv[] = {"program", "path/doesnt/exist/to/file.feature"};
  int argc = sizeof(argv) / sizeof(argv[0]);
  cuke::internal::terminal_arguments targs(argc, argv);
  ASSERT_TRUE(targs.get_options().files.empty());
}
TEST(options, file_path_does_exist)
{
  std::string path =
      std::format("{}/test_files/any.feature", unittests::test_dir());
  const char* argv[] = {"program", path.c_str()};
  int argc = sizeof(argv) / sizeof(argv[0]);
  cuke::internal::terminal_arguments targs(argc, argv);
  ASSERT_FALSE(targs.get_options().files.empty());
  EXPECT_EQ(targs.get_options().files[0].path, std::string(argv[1]));
}
