#include <gtest/gtest.h>

#include "../src/cwt/compiler/cucumber.hpp"

#define PRINT_STACK 1
#include "../src/cwt/debug.hpp"

using namespace cwt::details;

// TEST(compiler_single_line, feature_chunk_1_scenario)
// {
//   const char* script = R"*(
//   Feature: Hello World
  
//   Scenario: A Scenario
//   Given A Step

//   Scenario: Another Scenario
//   Given A Step
// )*";
//   compiler::cucumber cuke(script,{7});
//   cuke.set_options(options{.quiet = true});
//   cuke.compile();
//   chunk* f = cuke.get_chunk().constant(0).as<function>().get();
//   EXPECT_EQ(f->size(), 10);

//   disassemble_chunk(*f, "feature");
// }
// TEST(compiler_single_line, feature_chunk_3_scenarios)
// {
//   const char* script = R"*(
//   Feature: Hello World
  
//   Scenario: A Scenario
//   Given A Step

//   Scenario: Another Scenario
//   Given A Step
//   Scenario: Another Scenario
//   Given A Step
//   Scenario: Another Scenario
//   Given A Step
// )*";
//   compiler::cucumber cuke(script,{7,4,11});
//   cuke.set_options(options{.quiet = true});
//   cuke.compile();
//   chunk* f = cuke.get_chunk().constant(0).as<function>().get();
//   EXPECT_EQ(f->size(), 10);

//   disassemble_chunk(*f, "feature");
// }
// TEST(compiler_single_line, feature_chunk_0_scenarios)
// {
//   const char* script = R"*(
//   Feature: Hello World
  
//   Scenario: A Scenario
//   Given A Step

//   Scenario: Another Scenario
//   Given A Step
// )*";
//   compiler::cucumber cuke(script,{3});
//   cuke.set_options(options{.quiet = true});
//   cuke.compile();
//   chunk* f = cuke.get_chunk().constant(0).as<function>().get();
//   EXPECT_EQ(f->size(), 10);

//   disassemble_chunk(*f, "feature");
// }