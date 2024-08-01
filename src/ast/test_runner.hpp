#pragma once

#include <utility>
#include "ast.hpp"
#include "gtest/gtest.h"
#include "registry.hpp"
#include "../step_finder.hpp"
#include "test_results.hpp"

namespace cuke
{

[[nodiscard]] static bool skip_step()
{
  return !(results::last_scenario().steps.empty() ||
           results::last_step().status == results::test_status::passed);
}
static void update_feature_status()
{
  if (results::last_feature().status != results::test_status::passed)
  {
    return;
  }
  if (results::last_scenario().status != results::test_status::passed)
  {
    results::set_feature_to(results::test_status::failed);
  }
}
static void update_scenario_status()
{
  if (results::last_scenario().status != results::test_status::passed)
  {
    return;
  }
  if (results::last_step().status != results::test_status::passed)
  {
    results::last_scenario().status = results::test_status::failed;
  }
  update_feature_status();
}

template <typename... OptionalRow>
static void execute_step(cuke::ast::step_node step, OptionalRow&&... row)
{
  results::new_step();
  if (skip_step())
  {
    results::last_step().status = results::test_status::skipped;
    return;
  }
  cuke::internal::step_finder finder(step.name(),
                                     std::forward<OptionalRow>(row)...);
  auto it = finder.find(cuke::registry().steps().begin(),
                        cuke::registry().steps().end());
  if (it != cuke::registry().steps().end())
  {
    it->call(finder.values());
  }
  else
  {
    results::last_step().status = results::test_status::undefined;
  }
}

class test_runner
{
 public:
  void visit(const cuke::ast::feature_node&) { results::new_feature(); }
  void visit(const cuke::ast::scenario_node& scenario)
  {
    results::new_scenario();
    for (const cuke::ast::step_node& step : scenario.steps())
    {
      execute_step(step);
    }
    update_scenario_status();
  }
  void visit(const cuke::ast::scenario_outline_node& scenario_outline)
  {
    for (const cuke::ast::example_node& example : scenario_outline.examples())
    {
      for (std::size_t row = 1; row < example.table().row_count(); ++row)
      {
        results::new_scenario();
        for (const cuke::ast::step_node& step : scenario_outline.steps())
        {
          execute_step(step, example.table().hash_row(row));
        }
        update_scenario_status();
      }
    }
  }
};

}  // namespace cuke
