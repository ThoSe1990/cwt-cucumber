#pragma once

#include <cstddef>

#include "ast.hpp"
#include "test_results.hpp"
#include "registry.hpp"
#include "options.hpp"

namespace cuke
{
struct step_pipeline_context
{
  const ast::step_node& step;
  bool skip = false;
  results::test_status status = results::test_status::unknown;
};

struct scenario_pipeline_context
{
  const ast::scenario_node& scenario;
  const internal::tag_expression& tag_expression;
  bool skip = false;
  bool ignore = false;
  results::test_status status = results::test_status::unknown;
};

class test_runner
{
 public:
  test_runner()
      : m_tag_expression(program_arguments().get_options().tag_expression)
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
