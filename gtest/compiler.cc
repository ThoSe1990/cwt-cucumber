#include <gtest/gtest.h>

#include "../src/cwt/compiler.hpp"
#include "../src/cwt/chunk.hpp"

using namespace cwt::details;

TEST(compiler, init_object) { compiler c(""); }

TEST(compiler, empty_script)
{
  testing::internal::CaptureStderr();
  compiler c("");
  function func = c.compile();
  EXPECT_TRUE(c.error());
  EXPECT_EQ(std::string("[line 1] Error at end: Expect FeatureLine\n"),
            testing::internal::GetCapturedStderr());
}
TEST(compiler, invalid_begin)
{
  testing::internal::CaptureStderr();
  compiler c("asdfsadf");
  function func = c.compile();
  EXPECT_TRUE(c.error());
  EXPECT_EQ(std::string("[line 1] Error  at 'asdfsadf': Expect FeatureLine\n"),
            testing::internal::GetCapturedStderr());
}

TEST(compiler, main_chunk_name)
{
  compiler c("Feature:");
  function func = c.compile();
  EXPECT_EQ(func->name(), std::string{":1"});
}
TEST(compiler, main_chunk_code)
{
  compiler c("Feature:");
  function func_chunk = c.compile();

  EXPECT_EQ(func_chunk->size(), 9);

  EXPECT_EQ(func_chunk->at(0), to_uint(op_code::constant));
  EXPECT_EQ(func_chunk->at(1), 0);
  EXPECT_EQ(func_chunk->at(2), to_uint(op_code::define_var));
  EXPECT_EQ(func_chunk->at(3), 1);
  EXPECT_EQ(func_chunk->at(4), to_uint(op_code::get_var));
  EXPECT_EQ(func_chunk->at(5), 1);
  EXPECT_EQ(func_chunk->at(6), to_uint(op_code::call));
  EXPECT_EQ(func_chunk->at(7), 0);
  EXPECT_EQ(func_chunk->at(8), to_uint(op_code::func_return));
}
TEST(compiler, main_chunk_constants)
{
  compiler c("Feature:");
  function func_chunk = c.compile();

  EXPECT_EQ(func_chunk->constants_count(), 2);
  EXPECT_EQ(func_chunk->constant(0).type(), value_type::function);
  EXPECT_EQ(func_chunk->constant(1).type(), value_type::string);
}
TEST(compiler, feature_chunk)
{
  compiler c("Feature:");
  function func_chunk = c.compile();
  const function& feature = func_chunk->constant(0).as<function>();
  EXPECT_EQ(feature->size(), 1);
  EXPECT_EQ(feature->at(0), to_uint(op_code::func_return));
}