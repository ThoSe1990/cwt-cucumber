#pragma once

#include "ast.hpp"
#include "registry.hpp"
#include "../step_finder.hpp"

namespace cuke::internal
{
class test_runner
{
 public:
  // void subscribe(/*TODO*/){}
  void visit(const cuke::ast::feature_node&) {}
  void visit(const cuke::ast::scenario_node& scenario)
  {
    for (const cuke::ast::step_node& step : scenario.steps())
    {
      cuke::internal::step_finder finder(step.name());
      auto it = finder.find(cuke::registry::steps().begin(),
                            cuke::registry::steps().end());
      if (it != cuke::registry::steps().end())
      {
        it->call(finder.values());
      }
    }
  }
  void visit(const cuke::ast::scenario_outline_node&) {}

};

}  // namespace cuke::internal
