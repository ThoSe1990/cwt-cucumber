#include <gtest/gtest.h>

#include "../src/cwt/compiler/cucumber.hpp"

using namespace cwt::details;

TEST(compiler, init_object) 
{ compiler::cucumber c(""); }

TEST(compiler, empty_script)
{
  testing::internal::CaptureStderr();
  compiler::cucumber c("");
  c.compile();
  EXPECT_TRUE(c.error());
  EXPECT_EQ(std::string("[line 1] Error at end: Expect FeatureLine\n"),
            testing::internal::GetCapturedStderr());
}
TEST(compiler, invalid_begin)
{
  testing::internal::CaptureStderr();
  compiler::cucumber c("asdfsadf");
  c.compile();
  EXPECT_TRUE(c.error());
  EXPECT_EQ(std::string("[line 1] Error  at 'asdfsadf': Expect FeatureLine\n"),
            testing::internal::GetCapturedStderr());
}

TEST(compiler, main_chunk_name)
{
  testing::internal::CaptureStderr();
  compiler::cucumber c("Feature:");
  c.compile();
  function main = c.make_function();
  EXPECT_TRUE(c.error());
  EXPECT_EQ(main->name(), std::string{"script"});
  EXPECT_EQ(std::string("[line 1] Error at end: Expect ScenarioLine\n"),
            testing::internal::GetCapturedStderr());
}
TEST(compiler, main_chunk_other_language)
{
  testing::internal::CaptureStderr();
  const char* script = R"*(

# language: de

Funktion:
)*";
  compiler::cucumber c(script);
  c.compile();
  function main = c.make_function();
  EXPECT_TRUE(c.error());
  EXPECT_EQ(main->name(), std::string{"script"});
  EXPECT_EQ(std::string("[line 6] Error at end: Expect ScenarioLine\n"),
            testing::internal::GetCapturedStderr());
}
TEST(compiler, main_chunk_ignore_linebreaks)
{
  testing::internal::CaptureStderr();
  const char* script = R"*(
  
  Feature:
)*";
  compiler::cucumber c(script);
  c.compile();
  function main = c.make_function();
  EXPECT_TRUE(c.error());
  EXPECT_EQ(main->name(), std::string{"script"});
  EXPECT_EQ(std::string("[line 4] Error at end: Expect ScenarioLine\n"),
            testing::internal::GetCapturedStderr());
}
TEST(compiler, main_chunk_code)
{
  compiler::cucumber c("Feature:");
  c.compile();
  function main = c.make_function();

  EXPECT_EQ(main->size(), 9);

  EXPECT_EQ(main->at(0), to_uint(op_code::constant));
  EXPECT_EQ(main->at(1), 0);
  EXPECT_EQ(main->at(2), to_uint(op_code::define_var));
  EXPECT_EQ(main->at(3), 1);
  EXPECT_EQ(main->at(4), to_uint(op_code::get_var));
  EXPECT_EQ(main->at(5), 1);
  EXPECT_EQ(main->at(6), to_uint(op_code::call));
  EXPECT_EQ(main->at(7), 0);
  EXPECT_EQ(main->at(8), to_uint(op_code::func_return));
}
TEST(compiler, main_chunk_constants)
{
  compiler::cucumber c("Feature:");
  c.compile();
  function main = c.make_function();

  EXPECT_EQ(main->constants_count(), 2);
  EXPECT_EQ(main->constant(0).type(), value_type::function);
  EXPECT_EQ(main->constant(1).type(), value_type::string);
}
TEST(compiler, feature_chunk)
{
  compiler::cucumber c("Feature:");
  c.compile();
  function main = c.make_function();
  const function& feature = main->constant(0).as<function>();
  EXPECT_EQ(feature->name(), std::string(":1"));
  EXPECT_EQ(feature->back(), to_uint(op_code::func_return));
}
TEST(compiler, regular_scenario)
{
  testing::internal::CaptureStderr();
  const char* script = R"*(
  Feature: A Fancy Feature
  Scenario: A Scenario)*";
  compiler::cucumber c(script);
  c.compile();
  function main = c.make_function();
  EXPECT_EQ(main->name(), std::string{"script"});
  EXPECT_EQ(main->size(), 9);
  EXPECT_EQ(std::string("[line 3] Error at end: Expect StepLine\n"),
            testing::internal::GetCapturedStderr());
  EXPECT_TRUE(c.error());
}
