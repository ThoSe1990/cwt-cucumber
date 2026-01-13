#pragma once

#include <cstddef>
#include <unordered_set>

#include "ast.hpp"
#include "tags.hpp"
#include "options.hpp"

namespace cuke
{

class filter
{
 public:
  virtual ~filter() = default;
  virtual bool matches(const ast::scenario_node& scenario) const = 0;
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

 private:
  internal::tag_expression m_tag_expression;
  std::vector<std::unique_ptr<filter>> m_filters;
};

}  // namespace cuke
