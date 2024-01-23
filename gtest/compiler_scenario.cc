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
  testing::internal::CaptureStderr();
  cuke_compiler cuke(script);
  feature f(&cuke);
  scenario s(&f);
  s.compile();
  EXPECT_EQ(s.get_chunk().size(), 27);
}
