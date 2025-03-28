#include "ast.hpp"
#include "registry.hpp"
#include "test_runner.hpp"
#include "util_regex.hpp"

namespace cuke::ast
{
namespace
{
std::vector<std::string> replace_vars_in_doc_string(
    const std::vector<std::string> doc_string, const cuke::table::row& row)
{
  if (doc_string.empty())
  {
    return {};
  }

  std::vector<std::string> result;
  result.reserve(doc_string.size());

  for (const std::string& line : doc_string)
  {
    result.push_back(internal::replace_variables(line, row));
  }
  return result;
}
cuke::table replace_vars_in_tables(const cuke::table& data_table,
                                   const cuke::table::row& row)
{
  if (data_table.empty())
  {
    return {};
  }

  cuke::table result = data_table;
  for (cuke::value& cell : result.data())
  {
    if (cell.to_string().find('<') != std::string::npos)
    {
      cell = internal::replace_variables(cell.to_string(), row);
    }
  }
  return result;
}
}  // namespace
scenario_node::scenario_node(const scenario_outline_node& scenario_outline,
                             const example_node& examples, std::size_t row,
                             const background_node* background)
    // TODO: line should be the table line !
    : node(scenario_outline.keyword(),
           internal::replace_variables(scenario_outline.name(),
                                       examples.table().hash_row(row)),
           // TODO: refactor +row
           examples.line() + row, scenario_outline.file()),
      m_tags(scenario_outline.tags()),
      m_rule(scenario_outline.rule()),
      m_background(background)
{
  for (const auto& line : scenario_outline.description())
  {
    m_description.push_back(
        internal::replace_variables(line, examples.table().hash_row(row)));
  }

  for (const auto& tag : examples.tags())
  {
    if (std::find(m_tags.begin(), m_tags.end(), tag) == m_tags.end())
    {
      m_tags.push_back(tag);
    }
  }

  for (const auto& step : scenario_outline.steps())
  {
    cuke::internal::step_finder finder(step.name(),
                                       examples.table().hash_row(row));
    auto it = finder.find(cuke::registry().steps().begin(),
                          cuke::registry().steps().end());
    if (it != cuke::registry().steps().end())
    {
      const auto& current_row = examples.table().hash_row(row);

      m_steps.push_back(step_node(
          step.keyword(), internal::replace_variables(step.name(), current_row),
          step.file(), step.line(),
          replace_vars_in_doc_string(step.doc_string(), current_row),
          replace_vars_in_tables(step.data_table(), current_row),
          finder.values(), &*it));
    }
    else
    {
      m_steps.push_back(
          step_node(step.keyword(), step.name(), step.file(), step.line()));
    }
  }
}

}  // namespace cuke::ast
