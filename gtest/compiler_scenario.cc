#include <gtest/gtest.h>

#include "../src/cwt/compiler/scenario.hpp"

using namespace cwt::details;

TEST(compiler_scenario, chunk_size_wo_step)
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
  EXPECT_EQ(s.get_chunk().size(), 8);
  EXPECT_EQ(std::string("[line 4] Error at end: Expect StepLine\n"),
            testing::internal::GetCapturedStderr());
}

TEST(compiler_scenario, chunk_size)
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
  EXPECT_EQ(s.get_chunk().size(), 27);
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
  EXPECT_EQ(s.get_chunk().size(), 27);
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
