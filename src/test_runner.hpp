#pragma once

#include <cstddef>

#include "ast.hpp"
#include "tags.hpp"
#include "options.hpp"

namespace cuke
{
class test_runner : public ast::node_visitor
{
 public:
  test_runner()
      : m_tag_expression(
            get_program_args().is_set(program_args::arg::tags)
                ? get_program_args().get_value(program_args::arg::tags)
                : "")
  {
  }
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
};

}  // namespace cuke
