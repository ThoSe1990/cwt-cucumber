#pragma once

#include "ast.hpp"
#include "../lexer.hpp"

namespace cwt::details
{
class compiler
{
 public:
  compiler(std::string_view src) : m_lexer(src) 
  {
    m_head.infos.type = cuke::ast::node_type::gherkin_document;
  }
  cuke::ast::node compile()
  {
    m_lexer.advance();
    m_lexer.skip_linebreaks();
    // here we go ... 
    switch (m_lexer.current().type)
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
        m_lexer.error_at(m_lexer.current(), "Expect FeatureLine");
      }
    }
    return {};
  }

 private:
  cwt::details::lexer m_lexer;
  cuke::ast::node m_head;  
};
}  // namespace cwt::details::ast
