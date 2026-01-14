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
    if (m_patterns.empty())
    {
      return true;
    }

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
  bool find_pattern(std::string_view pattern,
                    std::string_view scenario_name) const
  {
    if (pattern.empty())
    {
      return scenario_name.empty();
    }

    if (pattern.front() == '*')
    {
      std::string_view rest = pattern.substr(1);
      return find_pattern(rest, scenario_name) ||
             (!scenario_name.empty() &&
              find_pattern(pattern, scenario_name.substr(1)));
    }

    if (pattern.front() == '?')
    {
      return !scenario_name.empty() &&
             find_pattern(pattern.substr(1), scenario_name.substr(1));
    }

    return !scenario_name.empty() && pattern.front() == scenario_name.front() &&
           find_pattern(pattern.substr(1), scenario_name.substr(1));
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
