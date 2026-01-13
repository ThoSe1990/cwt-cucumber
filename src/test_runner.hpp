#pragma once

#include <cstddef>

#include "ast.hpp"
#include "tags.hpp"

namespace cuke
{

class line_filter
{
 public:
  line_filter(std::unordered_set<std::size_t> lines) : m_lines(std::move(lines))
  {
  }

  bool matches(const ast::scenario_node& scenario) const
  {
    return m_lines.contains(scenario.line());
  }

 private:
  std::unordered_set<std::size_t> m_lines;
};

class name_filter
{
  bool matches(const ast::scenario_node& scenario) const { return true; }
};

class test_runner : public ast::node_visitor
{
 public:
  test_runner();
  void setup() const;
  void teardown() const;
  void run();

  void visit(const ast::feature_node& feature) override;
  void visit(const ast::scenario_node& scenario) override;
  void visit(const ast::scenario_outline_node& scenario_outline) override;

 private:
  void run_scenario(const ast::scenario_node& scenario) const;
  bool passes_filters(const ast::scenario_node& scenario) const;

 private:
  internal::tag_expression m_tag_expression;
  std::optional<line_filter> m_line_filter;
  std::optional<name_filter> m_name_filter;
};

}  // namespace cuke
