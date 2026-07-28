#pragma once

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "ast.hpp"
#include "options.hpp"
#include "table.hpp"
#include "value.hpp"

namespace cuke::internal
{

class lexer;

/**
 * @brief Grammar-production helpers driving \c cuke::parser. Not part of the
 * public step-definition API; defined in `parser.cpp` and unit-tested
 * directly in `gtest/ast.cc`.
 */
[[nodiscard]] std::pair<std::string, std::string> parse_keyword_and_name(
    lexer& lex, bool remove_colon);

[[nodiscard]] std::vector<std::string> parse_tags(lexer& lex);

[[nodiscard]] std::string trim(const std::string& str);

[[nodiscard]] std::vector<std::string> doc_string_to_vector(std::string_view s);

[[nodiscard]] std::string doc_string_type_from_token(std::string_view s);

[[nodiscard]] cuke::internal::doc_string parse_doc_string(lexer& lex);

[[nodiscard]] std::size_t advance_to_cell_end(lexer& lex);

[[nodiscard]] cuke::value parse_cell(lexer& lex,
                                     bool remove_quotes_from_strings);

[[nodiscard]] cuke::value_array parse_row(lexer& lex,
                                          bool remove_quotes_from_strings);

[[nodiscard]] std::pair<cuke::table, std::vector<std::size_t>> parse_table(
    lexer& lex, bool remove_quotes_from_strings);

[[nodiscard]] std::vector<cuke::ast::step_node> parse_steps(lexer& lex);

[[nodiscard]] cuke::ast::example_node parse_example(
    lexer& lex, std::vector<std::string>&& tags);

[[nodiscard]] std::unique_ptr<cuke::ast::scenario_outline_node>
make_scenario_outline(lexer& lex, std::vector<std::string>&& tags,
                      const std::optional<cuke::ast::rule_node>& rule);

[[nodiscard]] std::unique_ptr<cuke::ast::scenario_node> make_scenario(
    lexer& lex, std::vector<std::string>&& tags,
    const std::optional<cuke::ast::rule_node>& rule,
    const ast::background_node* background, const std::string& id_prefix);

[[nodiscard]] std::optional<cuke::ast::rule_node> parse_rule(lexer& lex);

[[nodiscard]] std::vector<std::unique_ptr<cuke::ast::node>> parse_scenarios(
    lexer& lex, const std::vector<std::string>& feature_tags,
    const ast::background_node* background, const std::string& feature_id);

[[nodiscard]] std::unique_ptr<cuke::ast::background_node> parse_background(
    lexer& lex);

[[nodiscard]] cuke::ast::feature_node parse_feature(lexer& lex);

}  // namespace cuke::internal

namespace cuke
{

/**
 * @class parser
 * @brief Recursive-descent parser turning a Gherkin `.feature` file (or
 * in-memory script) into an AST rooted at \c cuke::ast::gherkin_document.
 *
 * @details Internal part of the execution pipeline (Scanner -> Lexer ->
 * Parser -> AST -> test_runner). Not part of the public step-definition
 * API - exposed mainly for unit testing and tools that need to inspect a
 * parsed feature file.
 */
class parser
{
 public:
  [[nodiscard]] const cuke::ast::gherkin_document& head() const noexcept
  {
    return m_head;
  }

  [[nodiscard]] bool error() const noexcept { return m_error; }

  void parse_from_file(const internal::feature_file& file);
  void parse_from_file(std::string_view filepath);
  void parse_script(std::string_view script);

  [[nodiscard]] const ast::scenario_node* get_scenario_from_line(
      std::size_t line) const;

  void for_each_scenario(ast::node_visitor& visitor) const;

 private:
  void parse_impl(std::string_view script, std::string_view filename);

 private:
  cuke::ast::gherkin_document m_head;
  bool m_error{false};
};

}  // namespace cuke
