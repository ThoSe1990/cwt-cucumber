#pragma once

#include <cstddef>

#include "ast.hpp"
#include "tags.hpp"
#include "util.hpp"

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
 public:
  name_filter(std::string_view pattern)
      : m_patterns(internal::to_vector(pattern, ':'))
  {
  }

  bool matches(std::string_view scenario_name) const
  {
    for (const auto& pattern : m_patterns)
    {
      if (find_pattern(pattern, scenario_name))
      {
        return true;
      }
    }
    return false;
  }

 private:
  bool find_pattern(std::string_view pattern, std::string_view str) const
  {
    // TODO:
    // placeholders:
    // 1: *scenario name
    // 2: scenario name*
    // 3: *scenario name*
    // 4: scenario?name
    // colon separated (= multiple patterns):
    // 5: scenario name*:*outline*
    return pattern == str;
  }

 private:
  std::vector<std::string> m_patterns;
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
