#include "test_runner.hpp"

#include <memory>
#include <string>
#include <algorithm>

#include "ast.hpp"
#include "options.hpp"
#include "test_results.hpp"
#include "log.hpp"
#include "log_util.hpp"
#include "parser.hpp"
#include "context.hpp"

namespace cuke
{
namespace
{

struct step_pipeline_context
{
  const ast::step_node& step;
  results::step& result;
  const bool scenario_already_skpped;
};

struct scenario_pipeline_context
{
  const ast::scenario_node& scenario;
  const internal::tag_expression& tag_expression;
  bool skip_scenario = false;
  bool ignore = false;
  results::scenario& result;
};

[[nodiscard]] bool tags_valid(const scenario_pipeline_context& context)
{
  if (context.tag_expression.empty())
  {
    log::verbose_no_tags();
    return true;
  }
  const bool tag_evaluation =
      context.tag_expression.evaluate(context.scenario.tags());

  log::verbose_evaluate_tags(context.scenario, tag_evaluation,
                             context.tag_expression.expression());

  return tag_evaluation;
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

[[nodiscard]] bool has_undefined_steps(const std::vector<results::step>& steps)
{
  return std::any_of(
      steps.begin(), steps.end(), [](const auto& step)
      { return step.status == results::test_status::undefined; });
}

[[nodiscard]] bool has_failed_or_undefined_steps(
    const std::vector<results::step>& steps)
{
  return std::any_of(steps.begin(), steps.end(),
                     [](const auto& step)
                     {
                       return step.status == results::test_status::failed ||
                              step.status == results::test_status::undefined;
                     });
}

void update_scenario_status(scenario_pipeline_context& context)
{
  const auto& steps = results::scenarios_back().steps;
  if (context.skip_scenario)
  {
#ifdef UNDEFINED_STEPS_ARE_A_FAILURE
    if (has_undefined_steps(steps))
    {
      context.result.status = results::test_status::failed;
    }
    else
#endif  // UNDEFINED_STEPS_ARE_A_FAILURE
      context.result.status = results::test_status::skipped;
  }
  else if (internal::get_runtime_options().fail_scenario().is_set)
  {
    const std::string& msg =
        internal::get_runtime_options().fail_scenario().msg;
    log::error(msg, log::new_line);
    context.result.status = results::test_status::failed;
    for (results::step& step : context.result.steps)
    {
      step.error_msg = msg;
    }
  }
  else
  {
    if (has_failed_or_undefined_steps(steps))
    {
      context.result.status = results::test_status::failed;
    }
  }

  internal::get_runtime_options().reset_fail_scenario();
  results::test_results().add_scenario(context.result.status);
}

void skip_step(step_pipeline_context& context)
{
  const bool continue_on_failure_or_prev_step_failed = []()
  {
    if (get_program_args().is_set(program_args::arg::continue_on_failure))
    {
      return false;
    }

    const auto& this_scenario = results::scenarios_back();
    if (this_scenario.steps.size() <= 1)
    {
      return false;
    }
    else
    {
      const auto& last_step =
          results::scenarios_back()
              .steps[results::scenarios_back().steps.size() - 2];

      return last_step.status != results::test_status::passed;
    }
  }();

  if (continue_on_failure_or_prev_step_failed ||
      context.scenario_already_skpped ||
      internal::get_runtime_options().fail_scenario().is_set)
  {
    context.result.status = context.step.has_step_definition()
                                ? results::test_status::skipped
                                : results::test_status::undefined;
  }
}

void find_step(step_pipeline_context& context)
{
  if (!context.step.has_step_definition())
  {
    context.result.status = results::test_status::undefined;
    context.result.error_msg = "Undefined step";
  }
}

void hook_before_step(step_pipeline_context& context)
{
  context.result.source_location = context.step.source_location_definition();
  cuke::registry().run_hook_before_step();
}

void check_if_manual_fail_is_set(step_pipeline_context& context)
{
  if (internal::get_runtime_options().fail_step().is_set)
  {
    context.result.status = results::test_status::failed;
    context.result.error_msg = internal::get_runtime_options().fail_step().msg;
    log::error(internal::get_runtime_options().fail_step().msg, log::new_line);
  }
}

void call_step_and_hook_after(step_pipeline_context& context)
{
  context.step.call();
  cuke::registry().run_hook_after_step();
}
void teardown_step(step_pipeline_context& context)
{
  results::test_results().add_step(results::steps_back().status);
  internal::get_runtime_options().reset_fail_step();
  internal::get_runtime_options().sleep_if_has_delay();
  log::info(context.step, context.result.status);
}

// clang-format off
constexpr const std::array<void (*)(step_pipeline_context&), 6> step_pipeline = {
  skip_step,
  find_step,
  hook_before_step,
  check_if_manual_fail_is_set,
  call_step_and_hook_after,
  teardown_step
};
// clang-format on

void run_step(const ast::step_node& step, bool scenario_already_skpped)
{
  step_pipeline_context context{
      .step = step,
      .result = results::new_step(step),
      .scenario_already_skpped = scenario_already_skpped};

  for (const auto& pipeline_step : step_pipeline)
  {
    pipeline_step(context);
    if (context.result.status != results::test_status::passed)
    {
      teardown_step(context);
      break;
    }
  }
}

void verbose_start_print(scenario_pipeline_context& context)
{
  log::verbose("[   VERBOSE   ] ----------------------------------",
               log::new_line);
  log::verbose(std::format("[   VERBOSE   ] Scenario Start '{}' - File: {}:{}",
                           context.scenario.name(), context.scenario.file(),
                           context.scenario.line()),
               log::new_line);
}
void hook_before_scenario(scenario_pipeline_context& context)
{
  cuke::registry().run_hook_before(context.scenario.tags());
}
void is_scenario_ignored(scenario_pipeline_context& context)
{
  if (ignore_flag() || !tags_valid(context))
  {
    log::verbose_ignore();
    log::verbose_end();
    internal::get_runtime_options().skip_scenario(false);
    context.ignore = true;
    results::remove_last_scenario();
  }
}
void is_scenario_skipped(scenario_pipeline_context& context)
{
  context.skip_scenario = skip_flag() || get_program_args().is_set(
                                             cuke::program_args::arg::dry_run);

  if (context.skip_scenario)
  {
    log::verbose_skip();
    context.result.status = results::test_status::skipped;
  }
  log::info(context.scenario);
}
void run_background(scenario_pipeline_context& context)
{
  if (context.scenario.has_background())
  {
    for (const cuke::ast::step_node& step :
         context.scenario.background().steps())
    {
      run_step(step, context.skip_scenario);
    }
  }
}
void run_all_steps(scenario_pipeline_context& context)
{
  for (const cuke::ast::step_node& step : context.scenario.steps())
  {
    run_step(step, context.skip_scenario);
  }
}
void hook_after_scenario(scenario_pipeline_context& context)
{
  if (!internal::get_runtime_options().fail_scenario().is_set)
  {
    cuke::registry().run_hook_after(context.scenario.tags());
  }
}
void reset_user_context(scenario_pipeline_context&)
{
  cuke::internal::reset_context();
}
void verbose_end_print(scenario_pipeline_context& context)
{
  log::verbose_end();
  log::info(log::new_line);
}

// clang-format off
constexpr const std::array<void (*)(scenario_pipeline_context&), 10> scenario_pipeline = {
    verbose_start_print,
    hook_before_scenario, 
    is_scenario_ignored,
    is_scenario_skipped, 
    run_background,
    run_all_steps,
    hook_after_scenario,
    update_scenario_status,
    reset_user_context,
    verbose_end_print
};
// clang-format on

}  // namespace

test_runner::test_runner()
    : m_tag_expression(
          get_program_args().is_set(program_args::arg::tags)
              ? get_program_args().get_value(program_args::arg::tags)
              : "")
{
}
void test_runner::setup() const { cuke::registry().run_hook_before_all(); }
void test_runner::teardown() const { cuke::registry().run_hook_after_all(); }
void test_runner::run()
{
  for (const auto& feature : get_program_args().get_feature_files())
  {
    parser p;
    p.parse_from_file(feature.path);
    if (!feature.lines_to_run.empty())
    {
      m_line_filter.emplace(feature.lines_to_run);
    }
    p.for_each_scenario(*this);
    m_line_filter.reset();
  }
}

void test_runner::visit(const ast::feature_node& feature)
{
  results::new_feature(feature);
  log::info(feature);
}

void test_runner::visit(const ast::scenario_node& scenario)
{
  run_scenario(scenario);
}
void test_runner::visit(const ast::scenario_outline_node& scenario_outline)
{
  for (const auto& scenario : scenario_outline.concrete_scenarios())
  {
    run_scenario(scenario);
  }
}

bool test_runner::passes_filters(const ast::scenario_node& scenario) const
{
  if (m_line_filter && !m_line_filter->matches(scenario))
  {
    return false;
  }

  return true;
}

void test_runner::run_scenario(const ast::scenario_node& scenario) const
{
  if (!passes_filters(scenario))
  {
    return;
  }

  scenario_pipeline_context context{.scenario = scenario,
                                    .tag_expression = m_tag_expression,
                                    .result = results::new_scenario(scenario)};
  for (const auto& pipeline_step : scenario_pipeline)
  {
    pipeline_step(context);
    if (context.ignore) break;
  }
}

}  // namespace cuke
