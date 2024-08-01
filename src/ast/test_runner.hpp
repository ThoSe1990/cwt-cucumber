#pragma once

#include "ast.hpp"
#include "gtest/gtest.h"
#include "registry.hpp"
#include "../step_finder.hpp"
#include "test_results.hpp"

namespace cuke
{
class test_runner
{
 public:
  enum class state
  {
    idle = 0,
    skip
  };
  void visit(const cuke::ast::feature_node&)
  {
    results::feature f;
    results::new_feature(std::move(f));
  }
  void visit(const cuke::ast::scenario_node& scenario)
  {
    for (const cuke::ast::step_node& step : scenario.steps())
    {
      cuke::internal::step_finder finder(step.name());
      auto it = finder.find(cuke::registry().steps().begin(),
                            cuke::registry().steps().end());
      if (it != cuke::registry().steps().end())
      {
        it->call(finder.values());
      }
    }
  }
  void visit(const cuke::ast::scenario_outline_node& scenario_outline)
  {
    for (const cuke::ast::example_node& example : scenario_outline.examples())
    {
      for (std::size_t row = 1; row < example.table().row_count(); ++row)
      {
        for (const cuke::ast::step_node& step : scenario_outline.steps())
        {
          cuke::internal::step_finder finder(step.name(),
                                             example.table().hash_row(row));
          auto it = finder.find(cuke::registry().steps().begin(),
                                cuke::registry().steps().end());
          if (it != cuke::registry().steps().end())
          {
            it->call(finder.values());
          }
        }
      }
    }
  }

 private:
  state m_state{test_runner::state::idle};
};

}  // namespace cuke
