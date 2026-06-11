#pragma once

#include <memory>
#include <string>
#include <vector>

#include "ast.hpp"
#include "lexer.hpp"
#include "table.hpp"
#include "value.hpp"

namespace cuke::internal
{

[[nodiscard]] cuke::value parse_cell(lexer& lex,
                                     bool remove_quotes_from_strings);

[[nodiscard]] cuke::ast::example_node parse_example(
    lexer& lex, std::vector<std::string>&& tags);

[[nodiscard]] cuke::ast::feature_node parse_feature(lexer& lex);

[[nodiscard]] std::vector<std::unique_ptr<cuke::ast::node>> parse_scenarios(
    lexer& lex, const std::vector<std::string>& feature_tags,
    const ast::background_node* background, const std::string& feature_id);

[[nodiscard]] std::vector<cuke::ast::step_node> parse_steps(lexer& lex);

}  // namespace cuke::internal
