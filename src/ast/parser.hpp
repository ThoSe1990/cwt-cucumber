#include <string_view>

#include "ast.hpp"
#include "../token.hpp"
#include "../util.hpp"

namespace cwt::details
{

// void parse_scenario(lexer& lex, cuke::ast::node& scenario_node)
// {
//   using namespace cwt::details;

//   lex.skip_linebreaks();
//   lex.consume(token_type::scenario, "Expect: Scenario");
//   scenario_node.type = cuke::ast::node_type::scenario;
//   do
//   {
//     if (lex.match(token_type::step))
//     {
//       cuke::ast::node n;
//       // TODO
//       // step s(this);
//       // s.compile();
//       // node.steps ( ??? ) ;
//     }
//     else
//     {
//       lex.error_at(lex.current(), "Expect StepLine");
//       return;
//     }
//     lex.skip_linebreaks();
//   } while (lex.is_none_of(token_type::scenario, token_type::scenario_outline,
//                           token_type::tag, token_type::eof));

//   // scenario_node.children.push_back( step node? )
// }

cuke::ast::feature_node parse_feature(lexer& lex)
{
  if (!lex.match(cwt::details::token_type::feature))
  {
    lex.error_at(lex.current(), "Expect FeatureLine");
    // TODO 
    // return {};
  }
  // 1. keyword 
  std::string key = create_string(lex.previous().value);
  // 2. name 
  token begin = lex.current();
  lex.advance_to(token_type::linebreak, token_type::eof);
  token end = lex.previous();
  std::string name = create_string(begin, end);

  cuke::ast::feature_node feature(key, name);
  // 3. eol
  // 4. description
  // 5. eol 
  while (!lex.check(cwt::details::token_type::eof))
  {
    lex.skip_linebreaks();
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
        // parse_scenario(lex, node);
      }
      break;
      default:
      {
        // TODO error
        break;
      }
    }
    // feature_node.push_back(node);
    lex.advance();
  }
  return feature;
}

void parse_document(lexer& lex, cuke::ast::node& head)
{
  // lex.advance();
  // lex.skip_linebreaks();
  // cuke::ast::node feature;

  // while (lex.current().type == cwt::details::token_type::tag)
  // {
  //   // feature.tags.pushgback ..
  // }
  // if (lex.match(cwt::details::token_type::feature))
  // {
  //   parse_feature(lex, feature);
  // }
  // else 
  // {
  //   lex.error_at(lex.current(), "Expect FeatureLine");
  // }
  // head.children.push_back(feature);
}

}  // namespace cwt::details

namespace cuke
{
  
class parser
{
 public:
  // parser(std::string_view src) //: m_lexer(src)
  // {
    // m_head.type = cuke::ast::node_type::gherkin_document;
  // }
  const cuke::ast::gherkin_document& head() const noexcept { return m_head; }
  // void parse()
  // {
  //   m_lexer.advance();
  //   m_lexer.skip_linebreaks();
  //   cuke::ast::node node;

  //   switch (m_lexer.current().type)
  //   {
  //     case cwt::details::token_type::tag:
  //     {
  //       // TODO
  //       // push tags to feature node
  //       // node.tags.push(...);
  //     }
  //     break;
  //     case cwt::details::token_type::feature:
  //     {
  //       m_lexer.advance();
  //       parse_feature(m_lexer, node);
  //     }
  //     break;
  //     default:
  //     {
  //       m_lexer.error_at(m_lexer.current(), "Expect FeatureLine");
  //       return;
  //     }
  //   }
  //   // m_head.children.push_back(node);
  // }

  void parse_document(std::string_view document)
  {
    using namespace cwt::details;
    lexer lex(document);
    lex.advance();
    lex.skip_linebreaks();
    // std::vector<std::string> tags;
    // while (lex.check(cwt::details::token_type::tag)) 
    // {
    //   // tags.push_back(...)
    // }
  // better: 
  // auto tags = parse_tags(lex); 
  auto feature = parse_feature(lex); 

  m_head.m_type = cuke::ast::node_type::gherkin_document;
  m_head.m_feature = std::make_unique<cuke::ast::feature_node>(feature);

  }

 private:
  cuke::ast::gherkin_document m_head;
};

} // namespace cuke