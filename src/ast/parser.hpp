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

auto parse_keyword_and_name(lexer& lex)
{
  std::string key = create_string(lex.previous().value);
  token begin = lex.current();
  lex.advance_to(token_type::linebreak, token_type::eof);
  token end = lex.previous();
  lex.advance();
  std::string name = create_string(begin, end);
  return std::make_pair(key, name);
}

template <typename... Ts>
std::vector<std::string> parse_description(lexer& lex, Ts&&... terminators)
{
  std::vector<std::string> lines;
  while (!lex.match(std::forward<Ts>(terminators)...))
  {
    token begin = lex.current();
    lex.advance_to(token_type::linebreak);
    token end = lex.previous();
    lex.advance();
    lines.push_back(create_string(begin, end)); 
  }
  return lines;
}
std::vector<std::string> parse_tags(lexer& lex)
{
  std::vector<std::string> tags;
  while(lex.check(token_type::tag))
  {
    tags.push_back(create_string(lex.current().value));
    lex.advance();
  }
  lex.skip_linebreaks();
  return tags;
}
cuke::ast::feature_node parse_feature(lexer& lex)
{
  auto tags = parse_tags(lex);

  if (!lex.match(cwt::details::token_type::feature))
  {
    lex.error_at(lex.current(), "Expect FeatureLine");
    return cuke::ast::feature_node();
  }
  auto [key, name] = parse_keyword_and_name(lex);
  auto description = parse_description(
      lex, token_type::scenario, token_type::scenario_outline, token_type::tag,
      token_type::background, token_type::eof);

  auto scenarios = parse_scenarios(lex);

  return cuke::ast::feature_node(key, name, tags, description);
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

  void parse_script(std::string_view script)
  {
    using namespace cwt::details;
    lexer lex(script);
    lex.advance();
    lex.skip_linebreaks();
    m_head.make_feature(parse_feature(lex));
    if(lex.error())
    {
      m_head.clear();
    }
  }

 private:
  cuke::ast::gherkin_document m_head;
};

}  // namespace cuke