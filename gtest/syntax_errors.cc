#include <gtest/gtest.h>

extern "C" {
  #include "compiler.h"
}

TEST(syntax_errors, invalid_begin)
{
  testing::internal::CaptureStderr(); 

const char* script = R"*(
  this is wrong!
)*";
  EXPECT_TRUE(NULL == compile(script, "any_file"));

  const std::string error_print = testing::internal::GetCapturedStderr(); 
  EXPECT_STREQ("[line 2] Error at 'this': Expect 'Feature:'.\n", error_print.c_str());
}