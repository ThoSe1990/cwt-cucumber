#include <string_view>

#include "ast.hpp"
#include "../token.hpp"
#include "../util.hpp"

namespace cwt::details
{

auto parse_keyword_and_name(lexer& lex)
{
  std::string key = create_string(lex.current().value);
  lex.advance();
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
  while (!lex.check(std::forward<Ts>(terminators)...))
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
  while (lex.check(token_type::tag))
  {
    tags.push_back(create_string(lex.current().value));
    lex.advance();
  }
  lex.skip_linebreaks();
  return tags;
}
std::vector<std::unique_ptr<cuke::ast::node>> parse_scenarios(lexer& lex)
{
  std::vector<std::unique_ptr<cuke::ast::node>> scenarios;

  while (!lex.check(token_type::eof)) 
  {
    auto tags = parse_tags(lex);
    if (lex.check(token_type::scenario))
    {
      auto [key, name] = parse_keyword_and_name(lex);
      scenarios.push_back(std::make_unique<cuke::ast::scenario_node>());
    }
    else 
    {
      // TODO proper error 
      std::cout << "[Error] wrong token " << (int)lex.current().type << ' ' << lex.current().value << std::endl;
      break;
    }
  }
  return std::move(scenarios);
}
cuke::ast::feature_node parse_feature(lexer& lex)
{
  auto tags = parse_tags(lex);

  if (!lex.check(cwt::details::token_type::feature))
  {
    lex.error_at(lex.current(), "Expect FeatureLine");
    return cuke::ast::feature_node();
  }
  auto [key, name] = parse_keyword_and_name(lex);
  auto description = parse_description(
      lex, token_type::scenario, token_type::scenario_outline, token_type::tag,
      token_type::background, token_type::eof);

  auto scenarios = parse_scenarios(lex);

  return cuke::ast::feature_node(key, name, std::move(scenarios), tags,
                                 description);
}

}  // namespace cwt::details

namespace cuke
{

class parser
{
 public:
  const cuke::ast::gherkin_document& head() const noexcept { return m_head; }
  
  void parse_script(std::string_view script)
  {
    using namespace cwt::details;
    lexer lex(script);
    lex.advance();
    lex.skip_linebreaks();
    m_head.make_feature(parse_feature(lex));
    if (lex.error())
    {
      m_head.clear();
    }
  }

 private:
  cuke::ast::gherkin_document m_head;
};

}  // namespace cuke