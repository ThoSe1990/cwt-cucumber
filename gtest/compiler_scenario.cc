#include <gtest/gtest.h>

#include "../src/cwt/compiler/scenario.hpp"

using namespace cwt::details;

TEST(compiler_scenario, chunk_wo_step)
{
const char* script = R"*(
  Feature: Hello World
  Scenario: A Scenario
)*";
  testing::internal::CaptureStderr();
  cuke_compiler cuke(script);
  feature f(&cuke);
  scenario s(&f);
  s.compile();
  
  ASSERT_EQ(s.get_chunk().size(), 9);

  EXPECT_EQ(s.get_chunk().at(0), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(1), 1);
  EXPECT_EQ(s.get_chunk().at(2), to_uint(op_code::print));
  EXPECT_EQ(s.get_chunk().at(3), 0);
  EXPECT_EQ(s.get_chunk().at(4), to_uint(op_code::constant));
  EXPECT_EQ(s.get_chunk().at(5), 0);
  EXPECT_EQ(s.get_chunk().at(6), to_uint(op_code::println));
  EXPECT_EQ(s.get_chunk().at(7), 5);
  EXPECT_EQ(s.get_chunk().at(8), to_uint(op_code::init_scenario));

  EXPECT_EQ(std::string("[line 4] Error at end: Expect StepLine\n"),
            testing::internal::GetCapturedStderr());
}

TEST(compiler_scenario, chunk_w_step)
{
const char* script = R"*(
  Feature: Hello World
  Scenario: A Scenario
  Given A Step
)*";

  cuke_compiler cuke(script);
  feature f(&cuke);
  scenario s(&f);
  s.compile();
  EXPECT_EQ(s.get_chunk().size(), 19);
}

TEST(compiler_scenario, chunk_size_3_steps)
{
const char* script = R"*(
  Feature: Hello World
  Scenario: A Scenario
  Given First Step
  When Second Step
  Then Third Step
)*";

  cuke_compiler cuke(script);
  feature f(&cuke);
  scenario s(&f);
  s.compile();
  EXPECT_EQ(s.get_chunk().size(), 39);
}

TEST(compiler_scenario, chunk_size_2_scenarios)
{
const char* script = R"*(
  Feature: Hello World
  Scenario: A Scenario
  Given A Step
  Scenario: Another Scenario
  Given A Step
)*";
  
  cuke_compiler cuke(script);
  feature f(&cuke);
  f.compile();
  EXPECT_EQ(f.get_chunk().size(), 42);
}
