#include <gtest/gtest.h>

#include "../src/options.hpp"

using namespace cuke::internal;

TEST(help_screen, check_print_1)
{
  int test_argc = 1;
  const char* test_argv[] = {"some-executable"};

  cuke::cuke_args args;
  args.initialize(test_argc, test_argv);

  EXPECT_TRUE(args.get_options().flags.at(cuke::options::key::help));
}
TEST(help_screen, check_print_2)
{
  int test_argc = 2;
  const char* test_argv[] = {"some-executable", "-h"};

  cuke::cuke_args args;
  args.initialize(test_argc, test_argv);

  EXPECT_TRUE(args.get_options().flags.at(cuke::options::key::help));
}
TEST(help_screen, check_print_3)
{
  int test_argc = 2;
  const char* test_argv[] = {"some-executable", "--help"};

  cuke::cuke_args args;
  args.initialize(test_argc, test_argv);

  EXPECT_TRUE(args.get_options().flags.at(cuke::options::key::help));
}
TEST(help_screen, check_print_4)
{
  int test_argc = 3;
  const char* test_argv[] = {"some-executable", "some other arg", "--help"};

  cuke::cuke_args args;
  args.initialize(test_argc, test_argv);

  EXPECT_TRUE(args.get_options().flags.at(cuke::options::key::help));
}
