#include <gtest/gtest.h>

#include "../src/cwt/compiler.hpp"
#include "../src/cwt/chunk.hpp"

using namespace cwt::details;

TEST(compiler, init_object)
{
  compiler c; 
}

TEST(compiler, feature_only)
{
const char* script = R"*(
  Feature: some feature
)*";
  compiler c;
  function func = c.compile(script);
  EXPECT_EQ(func.name, std::string{"<script>"});
  EXPECT_EQ(func.chunk_data->size(), 4);
}