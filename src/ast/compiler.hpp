#pragma once

#include "ast.hpp"
#include "../parser.hpp"

namespace cwt::details
{
class compiler
{
 public:
  compiler(std::string_view src) : m_parser(src) 
  {
    m_head.infos.type = cuke::ast::node_type::gherkin_document;
  }
  cuke::ast::node compile()
  {
    m_parser.advance();
    m_parser.skip_linebreaks();
    // here we go ... 
    switch (m_parser.current().type)
    {
      case token_type::tag:
      {
        // TODO 
      }
      break;
      case token_type::feature:
      {
        cuke::ast::node node;
        node.infos.type = cuke::ast::node_type::feature; 
      }
      break;
      default:
      {
        m_parser.error_at(m_parser.current(), "Expect FeatureLine");
      }
    }
    return {};
  }

 private:
  cwt::details::parser m_parser;
  cuke::ast::node m_head;  
};
}  // namespace cwt::details::ast
