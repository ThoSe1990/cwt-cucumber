#pragma once

#include <utility>
#include "ast.hpp"
#include "registry.hpp"
#include "../step_finder.hpp"
#include "test_results.hpp"

namespace cuke
{

[[nodiscard]] static bool skip_step()
{
  return !(results::scenarios_back().steps.empty() ||
           results::steps_back().status == results::test_status::passed);
}
static void update_feature_status()
{
  if (results::features_back().status != results::test_status::passed)
  {
    return;
  }
  if (results::scenarios_back().status != results::test_status::passed)
  {
    results::set_feature_to(results::test_status::failed);
  }
}
static void update_scenario_status()
{
  if (results::scenarios_back().status != results::test_status::passed)
  {
    return;
  }
  if (results::steps_back().status != results::test_status::passed)
  {
    results::scenarios_back().status = results::test_status::failed;
  }
  update_feature_status();
}

template <typename... OptionalRow>
static void execute_step(cuke::ast::step_node step, OptionalRow&&... row)
{
  results::new_step();
  if (skip_step())
  {
    results::steps_back().status = results::test_status::skipped;
    return;
  }
  cuke::internal::step_finder finder(step.name(),
                                     std::forward<OptionalRow>(row)...);
  auto it = finder.find(cuke::registry().steps().begin(),
                        cuke::registry().steps().end());
  if (it != cuke::registry().steps().end())
  {
    cuke::registry().run_hook_before_step();
    it->call(finder.values());
    cuke::registry().run_hook_after_step();
  }
  else
  {
    results::steps_back().status = results::test_status::undefined;
  }
}

class test_runner
{
 public:
  void visit(const cuke::ast::feature_node& feature)
  {
    results::new_feature();

    if (feature.has_background())
    {
      m_background = &feature.background();
    }
  }
  void visit(const cuke::ast::scenario_node& scenario)
  {
    results::new_scenario();
    cuke::registry().run_hook_before(scenario.tags());
    run_background();
    for (const cuke::ast::step_node& step : scenario.steps())
    {
      execute_step(step);
    }
    cuke::registry().run_hook_after(scenario.tags());
    update_scenario_status();
  }
  void visit(const cuke::ast::scenario_outline_node& scenario_outline)
  {
    for (const cuke::ast::example_node& example : scenario_outline.examples())
    {
      for (std::size_t row = 1; row < example.table().row_count(); ++row)
      {
        results::new_scenario();
        cuke::registry().run_hook_before(scenario_outline.tags());
        run_background();
        for (const cuke::ast::step_node& step : scenario_outline.steps())
        {
          execute_step(step, example.table().hash_row(row));
        }
        cuke::registry().run_hook_after(scenario_outline.tags());
        update_scenario_status();
      }
    }
  }

 private:
  void run_background() const noexcept
  {
    if (has_background())
    {
      for (const cuke::ast::step_node& step : m_background->steps())
      {
        execute_step(step);
      }
    }
  }
  [[nodiscard]] bool has_background() const noexcept
  {
    return m_background != nullptr;
  }

 private:
  const cuke::ast::background_node* m_background = nullptr;
};

}  // namespace cuke
