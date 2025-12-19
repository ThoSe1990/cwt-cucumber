#pragma once

#include <cstddef>

#include "ast.hpp"
#include "tags.hpp"
#include "options.hpp"

namespace cuke
{
class test_runner
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
  void run() const;

  void visit(const cuke::ast::feature_node& feature) const;
  void visit(const cuke::ast::scenario_node& scenario) const;
  void visit(const cuke::ast::scenario_outline_node& scenario_outline) const;

 private:
  void run_scenario(const ast::scenario_node& scenario) const;

 private:
  const internal::tag_expression m_tag_expression;
};

}  // namespace cuke
