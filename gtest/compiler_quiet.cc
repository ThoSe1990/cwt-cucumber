#include <gtest/gtest.h>

#include "../src/cwt/compiler/scenario_outline.hpp"

#define PRINT_STACK 1
#include "../src/cwt/debug.hpp"

using namespace cwt::details;

// TODO

// TEST(compiler_quiet, quiet_chunk)
// {
// const char* script = R"*(
//   @tag1 @tag2
//   Feature: Hello World
//   Scenario: A Scenario
// )*";
//   compiler::cucumber cuke(script);
//   cuke.compile();
//   chunk* feature_chunk = cuke.get_chunk().constant(0).as<function>().get(); 
//   EXPECT_EQ(feature_chunk->size(), 31);
//   disassemble_chunk(cuke.get_chunk(), "script");
//   disassemble_chunk(*feature_chunk, "feature");
// }