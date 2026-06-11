#pragma once

#include <string_view>

#include "ast.hpp"
#include "options.hpp"

namespace cuke
{

class parser
{
 public:
  [[nodiscard]] const cuke::ast::gherkin_document& head() const noexcept;
  [[nodiscard]] bool error() const noexcept;

  void parse_from_file(const feature_file& file);
  void parse_from_file(std::string_view filepath);
  void parse_script(std::string_view script);

  const ast::scenario_node* get_scenario_from_line(std::size_t line) const;
  void for_each_scenario(ast::node_visitor& visitor) const;

 private:
  void parse_impl(std::string_view script, std::string_view filename);

 private:
  cuke::ast::gherkin_document m_head;
  bool m_error{false};
};

}  // namespace cuke
