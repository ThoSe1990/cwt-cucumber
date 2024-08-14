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
#include "../context.hpp"

// TODO:
// - cleanup ast dir
// - cleanup internal namespace
// - cleanup headers
// - print doc string (after step)
// - print table (after step)
// - fix defines for accessing docstrings and tables
// FIXME: in box examples
// - tags (tags are not evaluated in skip scenario ... )
// - tables (access crashes, probably table is not in value array)
// - doc strings (access crashes, same as in table) --> check step finder
// --> see example with doc string in execute_step this prevents crash
// todo here -> printer should print doc string and tables (it does not right
// now)
//

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
  results::test_results().add_scenario(results::scenarios_back().status);
  update_feature_status();
}

static void update_step_status()
{
  results::test_results().add_step(results::steps_back().status);
}

template <typename... OptionalRow>
static void execute_step(cuke::ast::step_node step, OptionalRow&&... row)
{
  if (skip_step())
  {
    results::new_step();
    results::steps_back().status = results::test_status::skipped;
    update_step_status();
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
    step.if_has_doc_string_do([&finder](const auto& doc_string)
                              { finder.values().push_back(doc_string); });
    step.if_has_table_do(
        [&finder](const cuke::table& t)
        { finder.values().push_back(std::make_unique<cuke::table>(t)); });
    it->call(finder.values());
    cuke::registry().run_hook_after_step();
  }
  else
  {
    results::steps_back().status = results::test_status::undefined;
  }
  update_step_status();
}

namespace details
{

template <typename T>
void print_file_line(const T& t)
{
  internal::println(internal::color::black, "  ", t.file(), ':', t.line());
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
  virtual void print(const cuke::ast::example_node& example,
                     std::size_t row) const noexcept
  {
  }
  virtual void print(const cuke::ast::step_node& step,
                     results::test_status status) const noexcept
  {
  }
};

class cuke_printer : public stdout_interface
{
 public:
  virtual void println() const noexcept override { internal::println(); }
  void print(const cuke::ast::feature_node& feature) const noexcept override
  {
    internal::print(feature.keyword(), ' ', feature.name());
    details::print_file_line(feature);
    internal::println();
  }
  void print(const cuke::ast::scenario_node& scenario) const noexcept override
  {
    internal::print(scenario.keyword(), ' ', scenario.name());
    details::print_file_line(scenario);
  }
  void print(const cuke::ast::scenario_outline_node& scenario_outline)
      const noexcept override
  {
    internal::print(scenario_outline.keyword(), ' ', scenario_outline.name());
    details::print_file_line(scenario_outline);
  }
  void print(const cuke::ast::example_node& example,
             std::size_t row) const noexcept override
  {
    internal::println("  With Examples:");
    auto [header, values] = example.table().to_string(0, row);
    internal::println("  ", header);
    internal::println("  ", values);
  }
  void print(const cuke::ast::step_node& step,
             results::test_status status) const noexcept override
  {
    internal::print(internal::to_color(status), internal::step_prefix(status),
                    step.keyword(), ' ', step.name());
    details::print_file_line(step);
    // TODO: print tables and doc strings
    step.if_has_doc_string_do(
        [](const std::vector<std::string>& doc_string)
        {
          internal::println("\"\"\"");
          for (const std::string& line : doc_string)
          {
            internal::println(line);
          }
          internal::println("\"\"\"");
        });
  }
};

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
    cuke::internal::reset_context();
    m_printer->println();
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
        m_printer->print(scenario_outline);
        cuke::registry().run_hook_before(scenario_outline.tags());
        run_background();
        for (const cuke::ast::step_node& step : scenario_outline.steps())
        {
          execute_step(step, example.table().hash_row(row));
          m_printer->print(step, results::steps_back().status);
        }
        m_printer->print(example, row);
        cuke::registry().run_hook_after(scenario_outline.tags());
        update_scenario_status();
        cuke::internal::reset_context();
        m_printer->println();
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
        m_printer->print(step, results::steps_back().status);
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
