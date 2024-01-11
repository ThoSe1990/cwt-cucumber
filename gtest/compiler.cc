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
  testing::internal::CaptureStderr();
  compiler c("Feature:");
  function main = c.compile();
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
  compiler c(script);
  function main = c.compile();
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
  compiler c(script);
  function main = c.compile();
  EXPECT_TRUE(c.error());
  EXPECT_EQ(main->name(), std::string{"script"});
  EXPECT_EQ(std::string("[line 4] Error at end: Expect ScenarioLine\n"),
          testing::internal::GetCapturedStderr());
}
TEST(compiler, main_chunk_code)
{
  compiler c("Feature:");
  function main = c.compile();

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
  compiler c("Feature:");
  function main = c.compile();

  EXPECT_EQ(main->constants_count(), 2);
  EXPECT_EQ(main->constant(0).type(), value_type::function);
  EXPECT_EQ(main->constant(1).type(), value_type::string);
}
TEST(compiler, feature_chunk)
{
  compiler c("Feature:");
  function main = c.compile();
  const function& feature = main->constant(0).as<function>();
  EXPECT_EQ(feature->name(), std::string(":1"));
  EXPECT_EQ(feature->back(), to_uint(op_code::func_return));
}
TEST(compiler, regular_scenario)
{
const char* script = R"*(
  Feature: A Fancy Feature
  Scenario: A Scenario
)*";
  compiler c(script);
  function main = c.compile();
  EXPECT_EQ(main->name(), std::string{"script"});
  EXPECT_EQ(main->size(), 9);
  EXPECT_TRUE(c.no_error());
}
TEST(compiler, feature_chunk_code)
{
const char* script = R"*(
  Feature: A Fancy Feature
  Scenario: A Scenario
)*";
  compiler c(script);
  function main = c.compile();
  const function& feature = main->constant(0).as<function>();
  EXPECT_EQ(feature->size(), 30);
  
  EXPECT_EQ(feature->at(0), to_uint(op_code::constant));
  EXPECT_EQ(feature->at(1), 0); // idx to string value to print
  EXPECT_EQ(feature->at(2), to_uint(op_code::print));
  EXPECT_EQ(feature->at(3), 0); // represents the color
  
  EXPECT_EQ(feature->at(4), to_uint(op_code::constant));
  EXPECT_EQ(feature->at(5), 1); // idx to string value to print
  EXPECT_EQ(feature->at(6), to_uint(op_code::println));
  EXPECT_EQ(feature->at(7), 0); // represents the color
  
  EXPECT_EQ(feature->at(8), to_uint(op_code::constant));
  EXPECT_EQ(feature->at(9), 2); // reset_context
  EXPECT_EQ(feature->at(10), to_uint(op_code::hook));
  EXPECT_EQ(feature->at(11), 0); // tag count (hook)

  EXPECT_EQ(feature->at(12), to_uint(op_code::constant));
  EXPECT_EQ(feature->at(13), 3); // before
  EXPECT_EQ(feature->at(14), to_uint(op_code::hook));
  EXPECT_EQ(feature->at(15), 0); // tag count (hook)

  EXPECT_EQ(feature->at(16), to_uint(op_code::constant));
  EXPECT_EQ(feature->at(17), 4); // function: scenario call
  EXPECT_EQ(feature->at(18),to_uint(op_code::define_var));
  EXPECT_EQ(feature->at(19), 5);
  EXPECT_EQ(feature->at(20),to_uint(op_code::get_var));
  EXPECT_EQ(feature->at(21), 5);
  EXPECT_EQ(feature->at(22), to_uint(op_code::call));
  EXPECT_EQ(feature->at(23), 0); 

  EXPECT_EQ(feature->at(24), to_uint(op_code::constant));
  EXPECT_EQ(feature->at(25), 6); // after
  EXPECT_EQ(feature->at(26), to_uint(op_code::hook));
  EXPECT_EQ(feature->at(27), 0); // after
  
  EXPECT_EQ(feature->at(28), to_uint(op_code::scenario_result));
  EXPECT_EQ(feature->at(29), to_uint(op_code::func_return));
}