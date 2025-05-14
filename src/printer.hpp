#pragma once
#include <string_view>

#include "log.hpp"
#include "ast.hpp"
#include "test_results.hpp"
#include "util_regex.hpp"

namespace cuke
{

class cuke_printer
{
 public:
  void println() const noexcept { log::info(log::new_line); }
  void print(std::string_view msg) const noexcept
  {
    log::verbose(log::black, msg, log::reset_color);
  }
  void println(std::string_view msg) const noexcept
  {
    log::verbose(log::black, msg, log::reset_color, log::new_line);
  }
  void print(const cuke::ast::feature_node& feature) const noexcept
  {
    log::info(feature.keyword(), ": ", feature.name());
    log::info(log::black, "  ", feature.file(), ':', feature.line(),
              log::reset_color, log::new_line, log::new_line);
  }
  void print(const cuke::ast::scenario_node& scenario) const noexcept
  {
    log::info(scenario.keyword(), ": ", scenario.name());
    log::info(log::black, "  ", scenario.file(), ':', scenario.line(),
              log::reset_color, log::new_line);
  }
  void print(const cuke::ast::scenario_outline_node& scenario_outline,
             const table::row& row) const noexcept
  {
    log::info(scenario_outline.keyword(), ": ",
              internal::replace_variables(scenario_outline.name(), row));
    log::info(log::black, "  ", scenario_outline.file(), ':',
              scenario_outline.line(), log::reset_color, log::new_line);
  }

  void print(const cuke::ast::step_node& step,
             results::test_status status) const noexcept
  {
    log::info(results::to_color(status), results::step_prefix(status),
              step.keyword(), ' ', step.name(), log::reset_color);
    log::info(log::black, "  ", step.file(), ':', step.line(), log::reset_color,
              log::new_line);
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
    log::info("\"\"\"", log::new_line);
    for (const std::string& line : doc_string)
    {
      log::info(line, log::new_line);
    }
    log::info("\"\"\"", log::new_line);
  }
  void print_table(const cuke::table& t) const noexcept
  {
    for (const std::string& row : t.to_string_array())
    {
      log::info("  ", row, log::new_line);
    }
  }
};

}  // namespace cuke
