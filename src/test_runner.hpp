#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <chrono>
#include <thread>

#include "ast.hpp"
#include "parser.hpp"
#include "registry.hpp"
#include "table.hpp"
#include "test_results.hpp"
#include "util.hpp"
#include "util_regex.hpp"
#include "context.hpp"
#include "options.hpp"

namespace cuke
{

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
  ::cuke::println(internal::color::black, "  ", t.file(), ':', t.line());
}

}  // namespace details

class stdout_interface
{
 public:
  virtual ~stdout_interface() = default;
  virtual void println() const noexcept {}
  virtual void print(const cuke::ast::feature_node& feature) const noexcept {}
  virtual void print(const cuke::ast::scenario_node& scenario) const noexcept {}
  virtual void print(const cuke::ast::scenario_outline_node& scenario_outline,
                     const table::row& row) const noexcept
  {
  }
  virtual void print(const cuke::ast::step_node& step,
                     results::test_status status) const noexcept
  {
  }
  virtual void print(const cuke::ast::step_node& step,
                     results::test_status status,
                     const table::row& row) const noexcept
  {
  }
};

class cuke_printer : public stdout_interface
{
 public:
  virtual void println() const noexcept override { ::cuke::println(); }
  void print(const cuke::ast::feature_node& feature) const noexcept override
  {
    ::cuke::print(feature.keyword(), ": ", feature.name());
    details::print_file_line(feature);
    println();
  }
  void print(const cuke::ast::scenario_node& scenario) const noexcept override
  {
    ::cuke::print(scenario.keyword(), ": ", scenario.name());
    details::print_file_line(scenario);
  }
  void print(const cuke::ast::scenario_outline_node& scenario_outline,
             const table::row& row) const noexcept override
  {
    ::cuke::print(scenario_outline.keyword(), ": ",
                  internal::replace_variables(scenario_outline.name(), row));
    details::print_file_line(scenario_outline);
  }
  void print(const cuke::ast::step_node& step, results::test_status status,
             const table::row& row) const noexcept override
  {
    const std::string step_w_example_variables =
        internal::replace_variables(step.name(), row);
    ::cuke::print(results::to_color(status), results::step_prefix(status),
                  step.keyword(), ' ', step_w_example_variables);
    details::print_file_line(step);
    if (!step.data_table().empty())
    {
      print_table(step.data_table());
    }
    if (!step.doc_string().empty())
    {
      print_doc_string(step.doc_string());
    }
  }
  void print(const cuke::ast::step_node& step,
             results::test_status status) const noexcept override
  {
    ::cuke::print(results::to_color(status), results::step_prefix(status),
                  step.keyword(), ' ', step.name());
    details::print_file_line(step);
    if (!step.data_table().empty())
    {
      print_table(step.data_table());
    }
    if (!step.doc_string().empty())
    {
      print_doc_string(step.doc_string());
    }
  }
  void print_doc_string(
      const std::vector<std::string>& doc_string) const noexcept
  {
    ::cuke::println("\"\"\"");
    for (const std::string& line : doc_string)
    {
      ::cuke::println(line);
    }
    ::cuke::println("\"\"\"");
  }
  void print_table(const cuke::table& t) const noexcept
  {
    for (const std::string& row : t.to_string_array())
    {
      ::cuke::println("  ", row);
    }
  }
};

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
    cuke::registry().run_hook_before(scenario.tags());
    results::new_scenario(scenario);
    const bool skip = skip_flag() || !tags_valid(scenario);
    if (skip)
    {
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
    cuke::registry().run_hook_after(scenario.tags());
    update_scenario_status(scenario.name(), scenario.file(), scenario.line(),
                           skip);
    cuke::internal::reset_context();
    m_printer->println();
  }

  [[nodiscard]] bool tags_valid(
      const ast::scenario_node& scenario) const noexcept
  {
    return m_tag_expression.empty() ||
           m_tag_expression.evaluate(scenario.tags());
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

    if (const char* env_p = std::getenv("CWT_CUCUMBER_STEP_DELAY"))
    {
      auto delay = std::stoi(env_p);
      std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
    m_printer->print(step, results::steps_back().status);
  }

 private:
  std::unique_ptr<stdout_interface> m_printer =
      std::make_unique<cuke_printer>();
  const internal::tag_expression m_tag_expression;
};

}  // namespace cuke
