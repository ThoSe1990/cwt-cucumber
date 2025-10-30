#include "log_util.hpp"

#include "util.hpp"
#include "util_regex.hpp"

namespace cuke::log
{
void verbose_end()
{
  verbose("[   VERBOSE   ] Scenario end", log::new_line);
  verbose("[   VERBOSE   ] ----------------------------------", new_line,
          log::new_line);
}
void verbose_no_tags()
{
  verbose("[   VERBOSE   ] No tags given, continuing", log::new_line);
}
void verbose_evaluate_tags(const ast::scenario_node& scenario,
                           bool tag_evaluation, const std::string& expression)
{
  verbose(std::format("[   VERBOSE   ] Scenario tags '{}'",
                      internal::to_string(scenario.tags())),
          new_line);
  verbose(
      std::format("                checked against tag expression '{}' -> {}",
                  expression,
                  tag_evaluation ? "'True', continuing with scenario"
                                 : "'False', stopping scenario"),
      new_line);
}
void verbose_skip()
{
  verbose("[   VERBOSE   ] Scenario skipped with 'skip_scenario'", new_line);
}
void verbose_ignore()
{
  verbose("[   VERBOSE   ] Scenario ignored with 'ignore_scenario'", new_line);
}

void info(const cuke::ast::feature_node& feature)
{
  info(feature.keyword(), ": ", feature.name());
  info(log::color::black());
  info("  ", feature.file(), ':', feature.line());
  info(log::color::reset());
  info(log::new_line, log::new_line);
}
void info(const cuke::ast::scenario_node& scenario)
{
  info(scenario.keyword(), ": ", scenario.name());
  info(log::color::black());
  info("  ", scenario.file(), ':', scenario.line());
  info(log::color::reset());
  info(log::new_line);
}
void info(const cuke::ast::scenario_outline_node& scenario_outline,
          const table::row& row)
{
  info(scenario_outline.keyword(), ": ",
       cuke::internal::replace_variables(scenario_outline.name(), row));
  info(color::black());
  info("  ", scenario_outline.file(), ':', scenario_outline.line());
  info(color::reset());
  info(new_line);
}

void info(const std::vector<std::string>& doc_string)
{
  info("\"\"\"", new_line);
  for (const std::string& line : doc_string)
  {
    info(line, new_line);
  }
  info("\"\"\"", new_line);
}
void info(const cuke::table& t)
{
  for (const std::string& row : t.to_string_array())
  {
    info("  ", row, new_line);
  }
}
void info(const cuke::ast::step_node& step, results::test_status status)
{
  info(results::to_color(status));
  info(results::step_prefix(status), step.keyword(), ' ', step.name());
  info(color::reset());

  info(color::black());
  info("  ", step.file(), ':', step.line());
  info(color::reset());
  info(new_line);

  if (!step.data_table().empty())
  {
    info(step.data_table());
  }
  if (!step.doc_string().empty())
  {
    info(step.doc_string());
  }
}

}  // namespace cuke::log
