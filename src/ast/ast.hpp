#pragma once

#include "../lexer.hpp"
#include "../token.hpp"

#include <string>
#include <vector>

namespace cuke::ast
{
enum class node_type
{
  gherkin_document = 0,
  feature,
  background,
  scenario,
  scenario_outline
};
struct info
{
  node_type type;
  struct
  {
    std::string file;
    std::size_t line;
  } location;
  std::string keyword;
  std::string value;
  std::string description;
};
struct step
{
  info infos;
  // TODO: datatable
  // TODO: doc string
};
struct node
{
  info infos;
  std::vector<node> children;
  // TODO tags
};

class parser
{
 public:
  parser(std::string_view src) : m_lexer(src)
  {
    m_head.infos.type = cuke::ast::node_type::gherkin_document;
  }
  cuke::ast::node compile()
  {
    m_lexer.advance();
    m_lexer.skip_linebreaks();
    cuke::ast::node node;
    // here we go ...
    switch (m_lexer.current().type)
    {
      case cwt::details::token_type::tag:
      {
        // TODO
      }
      break;
      case cwt::details::token_type::feature:
      {
        node.infos.type = cuke::ast::node_type::feature;
      }
      break;
      default:
      {
        m_lexer.error_at(m_lexer.current(), "Expect FeatureLine");
      }
    }
    return node;
  }

 private:
  cwt::details::lexer m_lexer;
  cuke::ast::node m_head;
};
}  // namespace cuke::ast
