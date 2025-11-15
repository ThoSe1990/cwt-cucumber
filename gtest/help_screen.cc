#include <gtest/gtest.h>

#include "../src/options.hpp"

using namespace cuke::internal;

TEST(help_screen, check_print_1)
{
  int test_argc = 1;
  const char* test_argv[] = {"some-executable"};

  cuke::program_args args;
  args.initialize(test_argc, test_argv);

  EXPECT_TRUE(args.is_set(cuke::program_args::key_t::help));
}
TEST(help_screen, check_print_2)
{
  int test_argc = 2;
  const char* test_argv[] = {"some-executable", "-h"};

  cuke::program_args args;
  args.initialize(test_argc, test_argv);

  EXPECT_TRUE(args.is_set(cuke::program_args::key_t::help));
}
TEST(help_screen, check_print_3)
{
  int test_argc = 2;
  const char* test_argv[] = {"some-executable", "--help"};

  cuke::program_args args;
  args.initialize(test_argc, test_argv);

  EXPECT_TRUE(args.is_set(cuke::program_args::key_t::help));
}
TEST(help_screen, check_print_4)
{
  int test_argc = 3;
  const char* test_argv[] = {"some-executable", "some other arg", "--help"};

  cuke::program_args args;
  args.initialize(test_argc, test_argv);

  EXPECT_TRUE(args.is_set(cuke::program_args::key_t::help));
}
