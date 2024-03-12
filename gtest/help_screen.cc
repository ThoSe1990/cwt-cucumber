#include <gtest/gtest.h>

#include "../src/options.hpp"

using namespace cwt::details;

TEST(help_screen, check_print_1)
{
  int test_argc = 1;
  const char* test_argv[] = {"some-executable"}; 
  
  EXPECT_TRUE(print_help(test_argc, test_argv));
}
TEST(help_screen, check_print_2)
{
  int test_argc = 2;
  const char* test_argv[] = {"some-executable", "-h"}; 
  
  EXPECT_TRUE(print_help(test_argc, test_argv));
}
TEST(help_screen, check_print_3)
{
  int test_argc = 2;
  const char* test_argv[] = {"some-executable", "--help"}; 
  
  EXPECT_TRUE(print_help(test_argc, test_argv));
}
TEST(help_screen, check_print_4)
{
  int test_argc = 2;
  const char* test_argv[] = {"some-executable", "some other arg", "--help"}; 
  
  EXPECT_FALSE(print_help(test_argc, test_argv));
}