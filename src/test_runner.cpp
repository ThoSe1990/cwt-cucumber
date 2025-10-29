#include "test_runner.hpp"

#include <string>
#include <algorithm>
#include <string_view>

#include "ast.hpp"
#include "test_results.hpp"
#include "util_regex.hpp"
#include "log.hpp"
#include "parser.hpp"
#include "util.hpp"
#include "context.hpp"

namespace cuke
{

namespace
{
void verbose_start(const ast::scenario_node& scenario)
{
  log::verbose("[   VERBOSE   ] ----------------------------------",
               log::new_line);
  log::verbose(std::format("[   VERBOSE   ] Scenario Start '{}' - File: {}:{}",
                           scenario.name(), scenario.file(), scenario.line()),
               log::new_line);
}
void verbose_end()
{
  log::verbose("[   VERBOSE   ] Scenario end", log::new_line);
  log::verbose("[   VERBOSE   ] ----------------------------------",
               log::new_line, log::new_line);
}
void verbose_no_tags()
{
  log::verbose("[   VERBOSE   ] No tags given, continuing", log::new_line);
}
void verbose_evaluate_tags(const ast::scenario_node& scenario,
                           bool tag_evaluation, const std::string& expression)
{
  log::verbose(std::format("[   VERBOSE   ] Scenario tags '{}'",
                           internal::to_string(scenario.tags())),
               log::new_line);
  log::verbose(
      std::format("                checked against tag expression '{}' -> {}",
                  expression,
                  tag_evaluation ? "'True', continuing with scenario"
                                 : "'False', stopping scenario"),
      log::new_line);
}
void verbose_skip()
{
  log::verbose("[   VERBOSE   ] Scenario skipped with 'skip_scenario'",
               log::new_line);
}
void verbose_ignore()
{
  log::verbose("[   VERBOSE   ] Scenario ignored with 'ignore_scenario'",
               log::new_line);
}

[[nodiscard]] bool tags_valid(const ast::scenario_node& scenario,
                              const internal::tag_expression& tag_expression)
{
  if (tag_expression.empty())
  {
    verbose_no_tags();
    return true;
  }
  bool tag_evaluation = tag_expression.evaluate(scenario.tags());
  verbose_evaluate_tags(scenario, tag_evaluation, tag_expression.expression());

  return tag_evaluation;
}

void log_helper(const cuke::ast::feature_node& feature)
{
  log::info(feature.keyword(), ": ", feature.name());
  log::info(log::color::black());
  log::info("  ", feature.file(), ':', feature.line());
  log::info(log::color::reset());
  log::info(log::new_line, log::new_line);
}
void log_helper(const cuke::ast::scenario_node& scenario)
{
  log::info(scenario.keyword(), ": ", scenario.name());
  log::info(log::color::black());
  log::info("  ", scenario.file(), ':', scenario.line());
  log::info(log::color::reset());
  log::info(log::new_line);
}
void log_helper(const cuke::ast::scenario_outline_node& scenario_outline,
                const table::row& row)
{
  log::info(scenario_outline.keyword(), ": ",
            internal::replace_variables(scenario_outline.name(), row));
  log::info(log::color::black());
  log::info("  ", scenario_outline.file(), ':', scenario_outline.line());
  log::info(log::color::reset());
  log::info(log::new_line);
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
  log::info(results::to_color(status));
  log::info(results::step_prefix(status), step.keyword(), ' ', step.name());
  log::info(log::color::reset());

  log::info(log::color::black);
  log::info("  ", step.file(), ':', step.line());
  log::info(log::color::reset());
  log::info(log::new_line);

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
  const auto& opts = program_arguments().get_options();
  const auto& last_scenario = results::scenarios_back();
  const auto& last_step = results::steps_back();

  if (opts.continue_on_failure) return false;
  if (last_scenario.steps.empty()) return false;
  return last_step.status != results::test_status::passed;
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
}

}  // namespace

bool skip_step(const ast::step_node& step, bool scenario_set_to_skip)
{
  if (skip_step() || scenario_set_to_skip ||
      internal::get_runtime_options().fail_scenario().is_set)
  {
    results::new_step(step);
    results::steps_back().status = step.has_step_definition()
                                       ? results::test_status::skipped
                                       : results::test_status::undefined;
    results::test_results().add_step(results::steps_back().status);
    internal::get_runtime_options().reset_fail_step();
    log_helper(step, results::steps_back().status);
    return true;
  }
  return false;
}
void setup_step(step_pipeline_context& context)
{
  context.result = results::new_step(context.step);
}

void find_step(step_pipeline_context& context)
{
  if (!context.step.has_step_definition())
  {
    context.result->status = results::test_status::undefined;
    context.result->error_msg = "Undefined step";
  }
}

void hook_before_step(step_pipeline_context& context)
{
  results::set_source_location(context.step.source_location_definition());
  cuke::registry().run_hook_before_step();
}

void check_if_manual_fail_is_set(step_pipeline_context& context)
{
  if (internal::get_runtime_options().fail_step().is_set)
  {
    context.result->status = results::test_status::failed;
    context.result->error_msg = internal::get_runtime_options().fail_step().msg;
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

  log_helper(context.step, results::steps_back().status);
}

// clang-format off
std::vector<void (*)(step_pipeline_context&)> step_pipeline = {
  setup_step,
  find_step,
  hook_before_step,
  check_if_manual_fail_is_set,
  call_step_and_hook_after
};
// clang-format on

void run_step(const ast::step_node& step, bool scenario_already_skpped)
{
  if (skip_step(step, scenario_already_skpped))
  {
    return;
  }

  step_pipeline_context context{.step = step};
  for (const auto& pipeline_step : step_pipeline)
  {
    pipeline_step(context);
    if (context.result->status != results::test_status::passed)
    {
      break;
    }
  }
  teardown_step(context);
}

void setup_scenario(scenario_pipeline_context& context)
{
  verbose_start(context.scenario);
}
void hook_before_scenario(scenario_pipeline_context& context)
{
  cuke::registry().run_hook_before(context.scenario.tags());
}
void is_scenario_ignored(scenario_pipeline_context& context)
{
  if (ignore_flag() || !tags_valid(context.scenario, context.tag_expression))
  {
    verbose_ignore();
    verbose_end();
    internal::get_runtime_options().skip_scenario(false);
    context.ignore = true;
  }
}
void is_scenario_skipped(scenario_pipeline_context& context)
{
  context.skip_scenario =
      skip_flag() || program_arguments().get_options().dry_run;
}
void init_scenario(scenario_pipeline_context& context)
{
  results::new_scenario(context.scenario);

  if (context.skip_scenario)
  {
    verbose_skip();
    results::scenarios_back().status = results::test_status::skipped;
  }

  log_helper(context.scenario);
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
void teardown_scenario(scenario_pipeline_context& context)
{
  update_scenario_status(context.scenario.name(), context.scenario.file(),
                         context.scenario.line(), context.skip_scenario);
  cuke::internal::reset_context();

  verbose_end();
  log::info(log::new_line);
}

// clang-format off
std::vector<void (*)(scenario_pipeline_context&)> scenario_pipeline = {
    setup_scenario,
    hook_before_scenario, 
    is_scenario_ignored,
    is_scenario_skipped, 
    init_scenario,        
    run_background,
    run_all_steps,       
    hook_after_scenario,  
    teardown_scenario
};
// clang-format on

void test_runner::setup() const { cuke::registry().run_hook_before_all(); }
void test_runner::teardown() const { cuke::registry().run_hook_after_all(); }
void test_runner::run() const
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
        if (const ast::scenario_node* scenario = p.get_scenario_from_line(line))
        {
          run_scenario(*scenario);
        }
      }
    }
  }
}

void test_runner::visit(const cuke::ast::feature_node& feature) const
{
  results::new_feature(feature);
  log_helper(feature);
}

void test_runner::visit(const cuke::ast::scenario_node& scenario) const
{
  run_scenario(scenario);
}
void test_runner::visit(
    const cuke::ast::scenario_outline_node& scenario_outline) const
{
  for (const auto& scenario : scenario_outline.concrete_scenarios())
  {
    run_scenario(scenario);
  }
}

void test_runner::run_scenario(const ast::scenario_node& scenario) const
{
  scenario_pipeline_context context{.scenario = scenario,
                                    .tag_expression = m_tag_expression};
  for (const auto& pipeline_step : scenario_pipeline)
  {
    pipeline_step(context);
    if (context.ignore) break;
  }
}

}  // namespace cuke
