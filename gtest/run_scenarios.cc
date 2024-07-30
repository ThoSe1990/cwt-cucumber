
#include <gtest/gtest.h>

#include "../src/ast/registry.hpp"
#include "../src/ast/parser.hpp"
#include "../src/step_finder.hpp"

namespace test_details
{
class scenario_runner
{
 public:
  scenario_runner()
  {
    call_count = 0;
    cuke::registry::push_step(cuke::internal::step(
        [](const cuke::value_array&) { scenario_runner::call_count++; },
        "a step"));
  }
  void visit(const cuke::ast::scenario_node& scenario)
  {
    for (const cuke::ast::step_node& step : scenario.steps())
    {
      execute_step(step);
    }
  }
  void visit(const cuke::ast::scenario_outline_node&) {}
  void execute_step(const cuke::ast::step_node& step)
  {
    cuke::internal::step_finder finder(step.name());
    auto it = std::find_if(cuke::registry::steps().begin(),
                           cuke::registry::steps().end(),
                           [&finder](const cuke::internal::step& s)
                           {
                             finder.reset_with_next_step(s.definition());
                             return finder.step_matches();
                           });
    if (it != cuke::registry::steps().end())
    {
      std::cout << "found step!" << std::endl;
      cuke::value_array v;
      it->call(v);
    }
  }

  static std::size_t call_count;
};
std::size_t scenario_runner::call_count = 0;
}  // namespace test_details

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
  EXPECT_EQ(test_details::scenario_runner::call_count, 1);
}
