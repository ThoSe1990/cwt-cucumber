#pragma once
#include <string_view>

#include "ast.hpp"
#include "test_results.hpp"
#include "util_regex.hpp"

namespace cuke
{
class stdout_interface
{
 public:
  virtual ~stdout_interface() = default;
  virtual void println() const noexcept {}
  virtual void print(std::string_view msg) const noexcept {}
  virtual void println(std::string_view msg) const noexcept {}
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
};

class cuke_printer : public stdout_interface
{
 public:
  void println() const noexcept override { ::cuke::println(); }
  void print(const cuke::ast::feature_node& feature) const noexcept override
  {
    ::cuke::print(feature.keyword(), ": ", feature.name());
    ::cuke::println(internal::color::black, "  ", feature.file(), ':',
                    feature.line());
    ::cuke::println();
  }
  void print(const cuke::ast::scenario_node& scenario) const noexcept override
  {
    ::cuke::print(scenario.keyword(), ": ", scenario.name());
    ::cuke::println(internal::color::black, "  ", scenario.file(), ':',
                    scenario.line());
  }
  void print(const cuke::ast::scenario_outline_node& scenario_outline,
             const table::row& row) const noexcept override
  {
    ::cuke::print(scenario_outline.keyword(), ": ",
                  internal::replace_variables(scenario_outline.name(), row));
    ::cuke::println(internal::color::black, "  ", scenario_outline.file(), ':',
                    scenario_outline.line());
  }

  void print(const cuke::ast::step_node& step,
             results::test_status status) const noexcept override
  {
    ::cuke::print(results::to_color(status), results::step_prefix(status),
                  step.keyword(), ' ', step.name());
    ::cuke::println(internal::color::black, "  ", step.file(), ':',
                    step.line());
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

class verbose_printer : public stdout_interface
{
 public:
  void println() const noexcept override { ::cuke::println(); }
  void print(std::string_view msg) const noexcept override
  {
    cuke::print(internal::color::black, msg);
  }
  void println(std::string_view msg) const noexcept override
  {
    cuke::println(internal::color::black, msg);
  }
};

}  // namespace cuke
