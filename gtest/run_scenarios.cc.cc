
#include <gtest/gtest.h>

#include "../src/ast/registry.hpp"
#include "../src/ast/parser.hpp"


namespace test_details {
class scenario_runner 
{
 public:
  scenario_runner() 
  {
    cuke::registry::push_step(
      cuke::internal::step([](const cuke::value_array&) {}, "a step"));
  }
  void visit(const cuke::ast::scenario_node&) {}
  void visit(const cuke::ast::scenario_outline_node&) {}
};
} // namespace test_details 
 
TEST(run_scenarios, run_step) 
{
  const char* script = R"*(
    Feature: a feature 
    Scenario: First Scenario 
    Given a step 
  )*";

  cuke::parser p;
  p.parse_script(script);
  test_details::scenario_runner visitor;
  p.for_each_scenario(visitor);

}

