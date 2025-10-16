#pragma once

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>

#include "ast.hpp"
#include "test_results.hpp"
#include "util_regex.hpp"
#include "log.hpp"
#include "parser.hpp"
#include "registry.hpp"
#include "util.hpp"
#include "context.hpp"
#include "options.hpp"

namespace cuke
{
namespace
{

void log_helper(const cuke::ast::feature_node& feature)
{
  log::info(feature.keyword(), ": ", feature.name());
  log::info(log::black, "  ", feature.file(), ':', feature.line(),
            log::reset_color, log::new_line, log::new_line);
}
void log_helper(const cuke::ast::scenario_node& scenario)
{
  log::info(scenario.keyword(), ": ", scenario.name());
  log::info(log::black, "  ", scenario.file(), ':', scenario.line(),
            log::reset_color, log::new_line);
}
void log_helper(const cuke::ast::scenario_outline_node& scenario_outline,
                const table::row& row)
{
  log::info(scenario_outline.keyword(), ": ",
            internal::replace_variables(scenario_outline.name(), row));
  log::info(log::black, "  ", scenario_outline.file(), ':',
            scenario_outline.line(), log::reset_color, log::new_line);
}

void log_helper_doc_string(const std::vector<std::string>& doc_string)
{
  log::info("\"\"\"", log::new_line);
  for (const std::string& line : doc_string)
  {
    log::info(line, log::new_line);
  }
  log::info("\"\"\"", log::new_line);
}
void log_helper_table(const cuke::table& t)
{
  for (const std::string& row : t.to_string_array())
  {
    log::info("  ", row, log::new_line);
  }
}
void log_helper(const cuke::ast::step_node& step, results::test_status status)
{
  log::info(results::to_color(status), results::step_prefix(status),
            step.keyword(), ' ', step.name(), log::reset_color);
  log::info(log::black, "  ", step.file(), ':', step.line(), log::reset_color,
            log::new_line);
  if (!step.data_table().empty())
  {
    log_helper_table(step.data_table());
  }
  if (!step.doc_string().empty())
  {
    log_helper_doc_string(step.doc_string());
  }
}

[[nodiscard]] bool ignore_flag()
{
  if (internal::get_runtime_options().ignore_scenario())
  {
    internal::get_runtime_options().ignore_scenario(false);
    return true;
  }
  return false;
}
[[nodiscard]] bool skip_flag()
{
  if (internal::get_runtime_options().skip_scenario())
  {
    internal::get_runtime_options().skip_scenario(false);
    return true;
  }
  return false;
}
[[nodiscard]] bool skip_step()
{
  return !program_arguments().get_options().continue_on_failure &&
         !(results::scenarios_back().steps.empty() ||
           results::steps_back().status == results::test_status::passed);
}
// FIXME: this is probably irrelevant: final result is determined by the counted
// scenarios not by the feature result. idk if a feature result is used anywhere
void update_feature_status()
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
void update_scenario_status(std::string_view name, std::string_view file,
                            std::size_t line, bool skipped)
{
  const auto& steps = results::scenarios_back().steps;

  if (skipped)
  {
#ifdef UNDEFINED_STEPS_ARE_A_FAILURE
    if (std::any_of(steps.begin(), steps.end(), [](const auto& step)
                    { return step.status == results::test_status::undefined; }))
    {
      results::scenarios_back().status = results::test_status::failed;
    }
    else
#endif  // UNDEFINED_STEPS_ARE_A_FAILURE
      results::scenarios_back().status = results::test_status::skipped;
  }
  else if (internal::get_runtime_options().fail_scenario().is_set)
  {
    const std::string& msg =
        internal::get_runtime_options().fail_scenario().msg;
    log::error(msg, log::new_line);
    results::scenarios_back().status = results::test_status::failed;
    std::for_each(results::scenarios_back().steps.begin(),
                  results::scenarios_back().steps.end(),
                  [&msg](results::step& step) { step.error_msg = msg; });
  }
  else
  {
    if (std::any_of(steps.begin(), steps.end(),
                    [](const auto& step)
                    {
                      return step.status == results::test_status::failed ||
                             step.status == results::test_status::undefined;
                    }))
    {
      {
        results::scenarios_back().status = results::test_status::failed;
      }
    }
  }

  internal::get_runtime_options().reset_fail_scenario();
  results::test_results().add_scenario(results::scenarios_back().status);
  update_feature_status();
}

void update_step_status()
{
  results::test_results().add_step(results::steps_back().status);
  internal::get_runtime_options().reset_fail_step();
}

}  // namespace

class test_runner
{
 public:
  test_runner()
      : m_tag_expression(program_arguments().get_options().tag_expression)
  {
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
    log_helper(feature);
  }
  void visit(const cuke::ast::scenario_node& scenario)
  {
    run_scenario(scenario);
  }
  void visit(const cuke::ast::scenario_outline_node& scenario_outline)
  {
    // FIXME: unused
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
    cuke::registry().run_hook_before(scenario.tags());

    if (ignore_flag() || !tags_valid(scenario))
    {
      verbose_ignore();
      verbose_end();
      // FIXME: lets refactor this here
      internal::get_runtime_options().skip_scenario(false);
      return;
    }

    // FIXME: better naming ...
    const bool skip = skip_flag() || program_arguments().get_options().dry_run;

    results::new_scenario(scenario);

    if (skip)
    {
      verbose_skip();
      results::scenarios_back().status = results::test_status::skipped;
    }

    log_helper(scenario);

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
    if (!internal::get_runtime_options().fail_scenario().is_set)
    {
      cuke::registry().run_hook_after(scenario.tags());
    }
    update_scenario_status(scenario.name(), scenario.file(), scenario.line(),
                           skip);
    cuke::internal::reset_context();

    verbose_end();
    log::info(log::new_line);
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
    // FIXME: these two calls to results::new_step are confusing.
    // right now skip_step() relies on it which means i can not put it before
    // the if statement. Will fix later
    //
    // Plus: skip_step and skip seem not very clear in the first place:
    //   - skip_step() -> if a prev. step failed (and the continue on failure
    //   flag wasn't set)
    //   - skip -> the skip flag from the user (cuke::skip_scenario()),
    //     these steps are as skipped in the report
    //
    if (skip_step() || skip ||
        internal::get_runtime_options().fail_scenario().is_set)
    {
      results::new_step(step);
      results::steps_back().status = step.has_step_definition()
                                         ? results::test_status::skipped
                                         : results::test_status::undefined;
      update_step_status();
      log_helper(step, results::steps_back().status);
      return;
    }
    results::new_step(step);
    if (step.has_step_definition())
    {
      results::set_source_location(step.source_location_definition());
      cuke::registry().run_hook_before_step();
      if (internal::get_runtime_options().fail_step().is_set)
      {
        results::steps_back().status = results::test_status::failed;
        results::steps_back().error_msg =
            internal::get_runtime_options().fail_step().msg;
        log::error(internal::get_runtime_options().fail_step().msg,
                   log::new_line);
      }
      else
      {
        step.call();
        cuke::registry().run_hook_after_step();
      }
    }
    else
    {
      results::steps_back().status = results::test_status::undefined;
      results::steps_back().error_msg = "Undefined step";
    }
    update_step_status();

    internal::get_runtime_options().sleep_if_has_delay();

    log_helper(step, results::steps_back().status);
  }

  void verbose_start(const ast::scenario_node& scenario) const noexcept
  {
    log::verbose("[   VERBOSE   ] ----------------------------------",
                 log::new_line);
    log::verbose(
        std::format("[   VERBOSE   ] Scenario Start '{}' - File: {}:{}",
                    scenario.name(), scenario.file(), scenario.line()),
        log::new_line);
  }
  void verbose_end() const noexcept
  {
    log::verbose("[   VERBOSE   ] Scenario end", log::new_line);
    log::verbose("[   VERBOSE   ] ----------------------------------",
                 log::new_line, log::new_line);
  }
  void verbose_no_tags() const noexcept
  {
    log::verbose("[   VERBOSE   ] No tags given, continuing", log::new_line);
  }
  void verbose_evaluate_tags(const ast::scenario_node& scenario,
                             bool tag_evaluation) const noexcept
  {
    log::verbose(std::format("[   VERBOSE   ] Scenario tags '{}'",
                             internal::to_string(scenario.tags())),
                 log::new_line);
    log::verbose(
        std::format("                checked against tag expression '{}' -> {}",
                    m_tag_expression.expression(),
                    tag_evaluation ? "'True', continuing with scenario"
                                   : "'False', stopping scenario"),
        log::new_line);
  }
  void verbose_skip() const noexcept
  {
    log::verbose("[   VERBOSE   ] Scenario skipped with 'skip_scenario'",
                 log::new_line);
  }
  void verbose_ignore() const noexcept
  {
    log::verbose("[   VERBOSE   ] Scenario ignored with 'ignore_scenario'",
                 log::new_line);
  }

 private:
  const internal::tag_expression m_tag_expression;
};

}  // namespace cuke
