#include <string_view>

#include "ast.hpp"
#include "../token.hpp"

namespace cwt::details
{

void parse_scenario(lexer& lex, cuke::ast::node& scenario_node)
{
  using namespace cwt::details;

  lex.skip_linebreaks();
  lex.consume(token_type::scenario, "Expect: Scenario");
  scenario_node.type = cuke::ast::node_type::scenario;
  do
  {
    if (lex.match(token_type::step))
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
  } while (lex.is_none_of(token_type::scenario, token_type::scenario_outline,
                          token_type::tag, token_type::eof));

  // scenario_node.children.push_back( step node? )
}

void parse_feature(lexer& lex, cuke::ast::node& feature_node)
{
  feature_node.type = cuke::ast::node_type::feature;

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
    lex.advance();
  }
}

void parse_document(lexer& lex, cuke::ast::node& head)
{
  lex.advance();
  lex.skip_linebreaks();
  cuke::ast::node feature;

  while (lex.current().type == cwt::details::token_type::tag)
  {
    // feature.tags.pushgback ..
  }
  if (lex.match(cwt::details::token_type::feature))
  {
    parse_feature(lex, feature);
  }
  else 
  {
    lex.error_at(lex.current(), "Expect FeatureLine");
  }
  head.children.push_back(feature);
}
cuke::ast::node make_ast(std::string_view document)
{
  cwt::details::lexer lex(document);
  cuke::ast::node head;
  head.type = cuke::ast::node_type::gherkin_document;
  parse_document(lex, head);
  return head;
}

class parser
{
 public:
  parser(std::string_view src) : m_lexer(src)
  {
    m_head.type = cuke::ast::node_type::gherkin_document;
  }
  cuke::ast::node& head() noexcept { return m_head; }
  void parse()
  {
    m_lexer.advance();
    m_lexer.skip_linebreaks();
    cuke::ast::node node;

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