#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <chrono>
#include <thread>

#include "ast.hpp"
#include "parser.hpp"
#include "registry.hpp"
#include "step_finder.hpp"
#include "table.hpp"
#include "test_results.hpp"
#include "util.hpp"
#include "util_regex.hpp"
#include "context.hpp"
#include "options.hpp"

namespace cuke
{

[[nodiscard]] static bool skip_scenario(const std::vector<std::size_t> lines,
                                        std::size_t line)
{
  if (internal::get_runtime_options().skip_scenario())
  {
    internal::get_runtime_options().skip_scenario(false);
    return true;
  }
  if (lines.empty())
  {
    return false;
  }

  return std::find(lines.begin(), lines.end(), line) == lines.end();
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
                                   std::size_t line)
{
  auto failed_already = []()
  { return results::scenarios_back().status != results::test_status::passed; };
  if (failed_already())
  {
    return;
  }

  if (results::steps_back().status != results::test_status::passed)
  {
    results::scenarios_back().status = results::test_status::failed;
    results::scenarios_back().name = name;
    results::scenarios_back().line = line;
  }
  results::test_results().add_scenario(results::scenarios_back().status);
  update_feature_status();
}

static void update_step_status()
{
  results::test_results().add_step(results::steps_back().status);
}

static void replace_vars_in_tables(cuke::table& data_table,
                                   const cuke::table::row& row)
{
  if (data_table.empty())
  {
    return;
  }

  for (cuke::value& cell : data_table.data())
  {
    if (cell.to_string().find('<') != std::string::npos)
    {
      cell = internal::replace_variables(cell.to_string(), row);
    }
    // const std::string& current = cell.to_string();
    // if (current.starts_with('<') && current.ends_with('>'))
    // {
    //   std::string var_name = current.substr(1, current.size() - 2);
    //   cell = row[var_name];
    // }
  }
}

// FIXME: I just noticed this copy here, which is not the best solution.
// due to the copy we can modify scenario outlines with their values.
// A long run solution for me is to create a kind of executable stack
// or another tree out of the AST, which is already validated and
// where during execution no operations
static void execute_step(
    cuke::ast::step_node step,
    std::optional<cuke::table::row> example_row = std::nullopt)
{
  if (skip_step())
  {
    results::new_step(step);
    results::steps_back().status = results::test_status::skipped;
    update_step_status();
    return;
  }
  results::new_step(step);
  if (example_row.has_value() && !step.data_table().empty())
  {
    replace_vars_in_tables(step.data_table(), example_row.value());
  }
  cuke::internal::step_finder finder(step.name(), example_row);
  auto it = finder.find(cuke::registry().steps().begin(),
                        cuke::registry().steps().end());
  if (it != cuke::registry().steps().end())
  {
    results::set_source_location(it->source_location());
    cuke::registry().run_hook_before_step();
    it->call(finder.values(), step.doc_string(), step.data_table());
    cuke::registry().run_hook_after_step();
    if (example_row.has_value())
    {
      const std::string step_w_example_variables =
          internal::replace_variables(step.name(), example_row.value());
      results::steps_back().name = step_w_example_variables;
    }
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
  virtual void print(
      const cuke::ast::scenario_outline_node& scenario_outline) const noexcept
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
  void print(const cuke::ast::scenario_outline_node& scenario_outline)
      const noexcept override
  {
    ::cuke::print(scenario_outline.keyword(), ": ", scenario_outline.name());
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
      cuke::table table_with_vars = step.data_table();
      replace_vars_in_tables(table_with_vars, row);
      print_table(table_with_vars);
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
      init_feature(feature);

      parser p;
      p.parse_from_file(feature.path);
      p.for_each_scenario(*this);
      clear_tags();
    }
  }

  void visit(const cuke::ast::feature_node& feature)
  {
    push_tags(feature.tags());
    results::new_feature(feature);
    m_printer->print(feature);
    if (feature.has_background())
    {
      m_background = &feature.background();
    }
  }
  void visit(const cuke::ast::scenario_node& scenario)
  {
    push_tags(scenario.tags());
    cuke::registry().run_hook_before(m_tags.container);
    results::new_scenario(scenario, m_tags.container);
    if (skip_scenario(m_lines, scenario.line()) || !tags_valid())
    {
      results::scenarios_back().status = results::test_status::skipped;
      pop_tags();
      return;
    }
    m_printer->print(scenario);
    run_background();
    for (const cuke::ast::step_node& step : scenario.steps())
    {
      execute_step(step);
      m_printer->print(step, results::steps_back().status);
    }
    cuke::registry().run_hook_after(m_tags.container);
    update_scenario_status(scenario.name(), scenario.file(), scenario.line());
    cuke::internal::reset_context();
    m_printer->println();
    pop_tags();
  }
  void visit(const cuke::ast::scenario_outline_node& scenario_outline)
  {
    push_tags(scenario_outline.tags());
    for (const cuke::ast::example_node& example : scenario_outline.examples())
    {
      push_tags(example.tags());
      if (!tags_valid())
      {
        pop_tags();
        continue;
      }
      for (std::size_t row = 1; row < example.table().row_count(); ++row)
      {
        results::new_scenario_outline(scenario_outline, row, m_tags.container);
        results::scenarios_back().line = example.line_table_begin() + row - 1;

        std::size_t row_file_line = example.line_table_begin() + row;
        cuke::registry().run_hook_before(m_tags.container);
        if (skip_scenario(m_lines, row_file_line))
        {
          results::scenarios_back().status = results::test_status::skipped;
          continue;
        }
        m_printer->print(scenario_outline);
        run_background();
        for (const cuke::ast::step_node& step : scenario_outline.steps())
        {
          execute_step(step, example.table().empty()
                                 ? std::nullopt
                                 : std::optional<cuke::table::row>(
                                       example.table().hash_row(row)));

          m_printer->print(step, results::steps_back().status,
                           example.table().hash_row(row));
        }
        cuke::registry().run_hook_after(m_tags.container);
        update_scenario_status(scenario_outline.name(), scenario_outline.file(),
                               row_file_line);
        cuke::internal::reset_context();
        m_printer->println();
      }
      pop_tags();
    }
    pop_tags();
  }
  void clear_tags() noexcept
  {
    m_tags.container.clear();
    m_tags.count_per_instance.clear();
  }

 private:
  void init_feature(const feature_file& feature) noexcept
  {
    m_lines = feature.lines_to_run;
    m_background = nullptr;
  }
  void run_background() const noexcept
  {
    if (has_background())
    {
      for (const cuke::ast::step_node& step : m_background->steps())
      {
        execute_step(step);
        m_printer->print(step, results::steps_back().status);
      }
    }
  }
  [[nodiscard]] bool tags_valid() const noexcept
  {
    if (m_tag_expression.empty())
    {
      return true;
    }
    return m_tag_expression.evaluate(this->m_tags.container);
  }
  [[nodiscard]] bool has_background() const noexcept
  {
    return m_background != nullptr;
  }
  void push_tags(const std::vector<std::string>& new_tags)
  {
    std::size_t inserted = 0;
    for (const auto& tag : new_tags)
    {
      if (std::find(m_tags.container.begin(), m_tags.container.end(), tag) ==
          m_tags.container.end())
      {
        m_tags.container.push_back(tag);
        ++inserted;
      }
    }
    m_tags.count_per_instance.push_back(inserted);
  }
  void pop_tags()
  {
    const std::size_t count = m_tags.count_per_instance.back();
    m_tags.count_per_instance.pop_back();
    if (count > 0)
    {
      m_tags.container.erase(m_tags.container.end() - count,
                             m_tags.container.end());
    }
  }

 private:
  const cuke::ast::background_node* m_background = nullptr;
  std::unique_ptr<stdout_interface> m_printer =
      std::make_unique<cuke_printer>();
  const internal::tag_expression m_tag_expression;
  std::vector<std::size_t> m_lines;
  struct
  {
    std::vector<std::string> container;
    std::vector<std::size_t> count_per_instance;
  } m_tags;
};

}  // namespace cuke
