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
TEST(compiler, main_feature_function)
{
  compiler c("Feature:");
  function func = c.compile();
  EXPECT_EQ(func.chunk_ptr->size(), 3);
  EXPECT_EQ(func.chunk_ptr->instruction(0), op_code::constant);
  EXPECT_EQ(func.chunk_ptr->at(1), 0);
  EXPECT_EQ(func.chunk_ptr->instruction(2), op_code::func_return);
  EXPECT_EQ(func.chunk_ptr->constants_count(), 1);
  EXPECT_EQ(func.chunk_ptr->constants_back().type(), value_type::function);
}