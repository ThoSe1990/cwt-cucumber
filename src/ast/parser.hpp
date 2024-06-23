#include <string_view>

#include "ast.hpp"
#include "../token.hpp"

namespace cwt::details
{

void parse_scenario(lexer& lex, cuke::ast::node scenario_node)
{
  lex.skip_linebreaks();
  // cuke::ast::node node;
  scenario_node.infos.type = cuke::ast::node_type::scenario;
  do
  {
    if (lex.match(cwt::details::token_type::step))
    {
      cuke::ast::node n;
      // TODO
      // step s(this);
      // s.compile();
      // node.steps ( ??? ) ;
    }
    else
    {
      lex.error_at(lex.current(), "Expect StepLine");
      return;
    }
    lex.skip_linebreaks();
  } while (lex.is_none_of(cwt::details::token_type::scenario,
                          cwt::details::token_type::scenario_outline,
                          cwt::details::token_type::tag,
                          cwt::details::token_type::eof));

  // scenario_node.children.push_back( step node? )
}

void parse_feature(lexer& lex, cuke::ast::node feature_node)
{
  while (!lex.check(cwt::details::token_type::eof))
  {
    lex.skip_linebreaks();
    cuke::ast::node node;
    switch (lex.current().type)
    {
      case cwt::details::token_type::tag:
      {
        // TODO
        // push tags to feature node
        // node.tags.push(...);
      }
      break;
      case cwt::details::token_type::scenario:
      {
        lex.advance();
        parse_scenario(lex, node);
      }
      break;
      default:
      {
        // TODO error
        break;
      }
    }

    feature_node.children.push_back(node);
  }
}

class parser
{
 public:
  parser(std::string_view src) : m_lexer(src)
  {
    m_head.infos.type = cuke::ast::node_type::gherkin_document;
  }
  cuke::ast::node& head() noexcept { return m_head; }
  void parse()
  {
    m_lexer.advance();
    m_lexer.skip_linebreaks();
    cuke::ast::node node;
    node.infos.type = cuke::ast::node_type::feature;
    switch (m_lexer.current().type)
    {
      case cwt::details::token_type::tag:
      {
        // TODO
        // push tags to feature node
        // node.tags.push(...);
      }
      break;
      case cwt::details::token_type::feature:
      {
        m_lexer.advance();
        parse_feature(m_lexer, node);
      }
      break;
      default:
      {
        m_lexer.error_at(m_lexer.current(), "Expect FeatureLine");
        return;
      }
    }
    m_head.children.push_back(node);
  }

 private:
  cwt::details::lexer m_lexer;
  cuke::ast::node m_head;
};

}  // namespace cwt::details