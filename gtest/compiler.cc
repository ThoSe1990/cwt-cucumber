#include <gtest/gtest.h>

#include "../src/cwt/compiler.hpp"
#include "../src/cwt/chunk.hpp"

using namespace cwt::details;

TEST(compiler, init_object)
{
  compiler c(""); 
}

TEST(compiler, function_name)
{
  compiler c("");
  function func = c.compile();
  EXPECT_EQ(func.name, std::string{"feature"});
}
TEST(compiler, feature_only)
{
  compiler c("Feature:");
  function func = c.compile();
  EXPECT_EQ(func.chunk_data->size(), 1);
  EXPECT_EQ(func.chunk_data->constants_count(), 1);
  EXPECT_EQ(func.chunk_data->constant(0).type(), value_type::function);
}