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
  // TODO refactor dry 
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
  void visit(const cuke::ast::scenario_outline_node& scenario_outline) 
  {
    for (const cuke::ast::example_node& example : scenario_outline.examples())
    {
      for (std::size_t row = 1 ; row < example.table().row_count() ; ++row)
      {
        for (const cuke::ast::step_node& step : scenario_outline.steps())
        {
          cuke::internal::step_finder finder(step.name(), example.table().hash_row(row));
          auto it = finder.find(cuke::registry::steps().begin(),
                                cuke::registry::steps().end());
          if (it != cuke::registry::steps().end())
          {
            it->call(finder.values());
          }
        }
      }
    }
  }

};

}  // namespace cuke::internal
