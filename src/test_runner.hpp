#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>

#include "ast.hpp"
#include "parser.hpp"
#include "registry.hpp"
#include "table.hpp"
#include "test_results.hpp"
#include "util.hpp"
#include "util_regex.hpp"
#include "context.hpp"
#include "options.hpp"
#include "printer.hpp"

namespace cuke
{

[[nodiscard]] static bool ignore_flag()
{
  if (internal::get_runtime_options().ignore_scenario())
  {
    internal::get_runtime_options().ignore_scenario(false);
    return true;
  }
  return false;
}
[[nodiscard]] static bool skip_flag()
{
  if (internal::get_runtime_options().skip_scenario())
  {
    internal::get_runtime_options().skip_scenario(false);
    return true;
  }
  return false;
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
static void update_scenario_status(std::string_view name, std::string_view file,
                                   std::size_t line, bool skipped)
{
  if (skipped)
  {
    results::scenarios_back().status = results::test_status::skipped;
  }
  else
  {
    const std::vector<results::step>& steps = results::scenarios_back().steps;
    for (const auto& step : steps)
    {
      if (step.status == results::test_status::failed ||
          step.status == results::test_status::undefined)
      {
        results::scenarios_back().status = results::test_status::failed;
      }
    }
  }

  results::test_results().add_scenario(results::scenarios_back().status);
  update_feature_status();
}

static void update_step_status()
{
  results::test_results().add_step(results::steps_back().status);
}

namespace details
{

template <typename T>
void print_file_line(const T& t)
{
  ::cuke::print(internal::color::black, "  ", t.file(), ':', t.line());
}
}  // namespace details

class test_runner
{
 public:
  test_runner()
      : m_tag_expression(program_arguments().get_options().tag_expression)
  {
    if (program_arguments().get_options().quiet)
    {
      m_printer.reset(std::make_unique<stdout_interface>().release());
    }
    if (program_arguments().get_options().verbose)
    {
      m_verbose_printer.reset(std::make_unique<verbose_printer>().release());
    }
  }
  void setup() { cuke::registry().run_hook_before_all(); }

  void teardown() { cuke::registry().run_hook_after_all(); }

  void run()
  {
    for (const auto& feature : program_arguments().get_options().files)
    {
      parser p;
      p.parse_from_file(feature.path);
      if (feature.lines_to_run.empty())
      {
        p.for_each_scenario(*this);
      }
      else
      {
        visit(p.head().feature());
        for (const std::size_t line : feature.lines_to_run)
        {
          if (const ast::scenario_node* scenario =
                  p.get_scenario_from_line(line))
          {
            run_scenario(*scenario);
          }
        }
      }
    }
  }

  void visit(const cuke::ast::feature_node& feature)
  {
    results::new_feature(feature);
    m_printer->print(feature);
  }
  void visit(const cuke::ast::scenario_node& scenario)
  {
    run_scenario(scenario);
  }
  void visit(const cuke::ast::scenario_outline_node& scenario_outline)
  {
    int i = 0;
    for (const auto& scenario : scenario_outline.concrete_scenarios())
    {
      run_scenario(scenario);
    }
  }

 private:
  void run_scenario(const ast::scenario_node& scenario) const
  {
    verbose_start(scenario);
    cuke::registry().run_hook_before(scenario.tags(), m_verbose_printer);

    if (ignore_flag())
    {
      verbose_ignore();
      verbose_end();
      return;
    }

    if (tags_valid(scenario))
    {
      results::new_scenario(scenario);

      const bool skip = skip_flag();
      if (skip)
      {
        verbose_skip();
        results::scenarios_back().status = results::test_status::skipped;
      }

      m_printer->print(scenario);

      if (scenario.has_background())
      {
        for (const cuke::ast::step_node& step : scenario.background().steps())
        {
          run_step(step, skip);
        }
      }
      for (const cuke::ast::step_node& step : scenario.steps())
      {
        run_step(step, skip);
      }

      cuke::registry().run_hook_after(scenario.tags(), m_verbose_printer);
      update_scenario_status(scenario.name(), scenario.file(), scenario.line(),
                             skip);
      cuke::internal::reset_context();
    }
    verbose_end();
    m_printer->println();
  }

  [[nodiscard]] bool tags_valid(
      const ast::scenario_node& scenario) const noexcept
  {
    if (m_tag_expression.empty())
    {
      verbose_no_tags();
      return true;
    }
    bool tag_evaluation = m_tag_expression.evaluate(scenario.tags());
    verbose_evaluate_tags(scenario, tag_evaluation);

    return tag_evaluation;
  }

  void run_step(const ast::step_node& step, bool skip) const
  {
    if (skip_step() || skip)
    {
      results::new_step(step);
      results::steps_back().status = step.has_step_definition()
                                         ? results::test_status::skipped
                                         : results::test_status::undefined;
      update_step_status();
      m_printer->print(step, results::steps_back().status);
      return;
    }
    results::new_step(step);
    if (step.has_step_definition())
    {
      results::set_source_location(step.source_location_definition());
      cuke::registry().run_hook_before_step();
      step.call();
      cuke::registry().run_hook_after_step();
    }
    else
    {
      results::steps_back().status = results::test_status::undefined;
      results::steps_back().error_msg = "Undefined step";
    }
    update_step_status();

    internal::get_runtime_options().sleep_if_has_delay();

    m_printer->print(step, results::steps_back().status);
  }

  void verbose_start(const ast::scenario_node& scenario) const noexcept
  {
    m_verbose_printer->println(
        "[   VERBOSE   ] ----------------------------------");
    m_verbose_printer->println(
        std::format("[   VERBOSE   ] Scenario Start '{}' - File: {}:{}",
                    scenario.name(), scenario.file(), scenario.line()));
  }
  void verbose_end() const noexcept
  {
    m_verbose_printer->println("[   VERBOSE   ] Scenario end");
    m_verbose_printer->println(
        "[   VERBOSE   ] ----------------------------------");
    m_verbose_printer->println();
  }
  void verbose_no_tags() const noexcept
  {
    m_verbose_printer->println("[   VERBOSE   ] No tags given, continuing");
  }
  void verbose_evaluate_tags(const ast::scenario_node& scenario,
                             bool tag_evaluation) const noexcept
  {
    m_verbose_printer->println(
        std::format("[   VERBOSE   ] Scenario tags '{}'",
                    internal::to_string(scenario.tags())));
    m_verbose_printer->println(
        std::format("                checked against tag expression '{}' -> {}",
                    m_tag_expression.expression(),
                    tag_evaluation ? "'True', continuing with scenario"
                                   : "'False', stopping scenario"));
  }
  void verbose_skip() const noexcept
  {
    m_verbose_printer->println(
        "[   VERBOSE   ] Scenario skipped with 'skip_scenario'");
  }
  void verbose_ignore() const noexcept
  {
    m_verbose_printer->println(
        "[   VERBOSE   ] Scenario ignored with 'ignore_scenario'");
  }

 private:
  std::unique_ptr<stdout_interface> m_printer =
      std::make_unique<cuke_printer>();
  std::unique_ptr<stdout_interface> m_verbose_printer =
      std::make_unique<stdout_interface>();
  const internal::tag_expression m_tag_expression;
};

}  // namespace cuke
