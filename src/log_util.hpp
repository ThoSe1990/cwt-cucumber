#include "ast.hpp"
#include "test_results.hpp"

namespace cuke::log
{
void verbose_end();
void verbose_no_tags();
void verbose_evaluate_tags(const ast::scenario_node& scenario,
                           bool tag_evaluation, const std::string& expression);
void verbose_skip();
void verbose_ignore();
void info(const cuke::ast::feature_node& feature);
void info(const cuke::ast::scenario_node& scenario);
void info(const cuke::ast::scenario_outline_node& scenario_outline,
          const table::row& row);
void info(const std::vector<std::string>& doc_string);
void info(const cuke::table& t);
void info(const cuke::ast::step_node& step, results::test_status status);

}  // namespace cuke::log
