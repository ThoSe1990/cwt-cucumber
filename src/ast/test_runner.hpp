#pragma once

#include <algorithm>
#include <memory>
#include <utility>
#include "ast.hpp"
#include "registry.hpp"
#include "../step_finder.hpp"
#include "test_results.hpp"
#include "../util.hpp"
#include "../options.hpp"

namespace cuke
{

[[nodiscard]] static bool skip_scenario(const internal::feature_file* file,
                                        std::size_t line)
{
  if (file == nullptr || file->lines_to_compile.empty())
  {
    return false;
  }
  return std::find(file->lines_to_compile.begin(), file->lines_to_compile.end(),
                   line) == file->lines_to_compile.end();
}
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
  if (skip_step())
  {
    results::new_step();
    results::steps_back().status = results::test_status::skipped;
    return;
  }
  results::new_step();
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

class stdout_interface
{
 public:
  virtual ~stdout_interface() = default;
  virtual void print(const cuke::ast::feature_node& feature) const noexcept {}
  virtual void print(const cuke::ast::scenario_node& scenario) const noexcept {}
  virtual void print(
      const cuke::ast::scenario_outline_node& scenario_outline) const noexcept
  {
  }
  virtual void print(const cuke::ast::example_node& example) const noexcept {}
  virtual void print(const cuke::ast::step_node& step,
                     results::test_status status) const noexcept
  {
  }
};

class cuke_printer : public stdout_interface
{
 public:
  void print(const cuke::ast::feature_node& feature) const noexcept override
  {
    internal::println(feature.keyword(), ' ', feature.name());
    internal::println();
  }
  void print(const cuke::ast::scenario_node& scenario) const noexcept override
  {
    internal::println(scenario.keyword(), ' ', scenario.name());
  }
  void print(const cuke::ast::scenario_outline_node& scenario_outline)
      const noexcept override
  {
    internal::println(scenario_outline.keyword(), ' ', scenario_outline.name());
  }
  void print(const cuke::ast::example_node& example) const noexcept override {}
  void print(const cuke::ast::step_node& step,
             results::test_status status) const noexcept override
  {
    internal::print(internal::to_color(status), internal::step_prefix(status),
                    step.keyword(), ' ', step.name(), "  ");
    internal::println(internal::color::black, step.file(), ':', step.line());
  }
};

// TODO:
// - unittests for run w and w/o printer
// - cleanup: remove internal namespace
// - cleanup: remove ast directory

class test_runner
{
 public:
  test_runner() = default;
  test_runner(const internal::feature_file* file) : m_file(file) {}

  void set_quiet()
  {
    m_printer.reset(std::make_unique<stdout_interface>().release());
  }

  void visit(const cuke::ast::feature_node& feature)
  {
    results::new_feature();
    m_printer->print(feature);
    if (feature.has_background())
    {
      m_background = &feature.background();
    }
  }
  void visit(const cuke::ast::scenario_node& scenario)
  {
    results::new_scenario();
    if (skip_scenario(m_file, scenario.line()))
    {
      results::scenarios_back().status = results::test_status::skipped;
      return;
    }
    m_printer->print(scenario);
    cuke::registry().run_hook_before(scenario.tags());
    run_background();
    for (const cuke::ast::step_node& step : scenario.steps())
    {
      execute_step(step);
      m_printer->print(step, results::steps_back().status);
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
        if (skip_scenario(m_file, example.line_table_begin() + row))
        {
          results::scenarios_back().status = results::test_status::skipped;
          continue;
        }
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
  std::unique_ptr<stdout_interface> m_printer =
      std::make_unique<cuke_printer>();
  const internal::feature_file* m_file = nullptr;
};

}  // namespace cuke
