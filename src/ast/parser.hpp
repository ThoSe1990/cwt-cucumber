#pragma once

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
std::vector<std::string> parse_doc_string(lexer& lex)
{
  if (lex.match(token_type::doc_string))
  {
    std::cout << lex.previous().value << std::endl;
    // TODO string split by linebreak;
    // return vector<string> 1 ... last-1 
  }
  else 
  {
    std::cout << "no doc string ... " << std::endl;
  }
  return {};
}
std::vector<cuke::ast::step_node> parse_steps(lexer& lex)
{
  using namespace cwt::details;
  std::vector<cuke::ast::step_node> steps;
  do
  {
    if (lex.check(token_type::step))
    {
      const std::size_t line = lex.current().line;
      auto [key, name] = parse_keyword_and_name(lex);
      std::vector<std::string> doc_string = parse_doc_string(lex);
      // TODO: data table 
      steps.push_back(cuke::ast::step_node(std::move(key), std::move(name),
                                           lex.filepath(), line, std::move(doc_string)));
    }
    else
    {
      lex.error_at(lex.current(), "Expect Step line");
      return {};
    }
  } while (lex.is_none_of(token_type::scenario, token_type::scenario_outline,
                          token_type::tag, token_type::eof));
  return steps;
}

std::vector<std::unique_ptr<cuke::ast::node>> parse_scenarios(lexer& lex)
{
  std::vector<std::unique_ptr<cuke::ast::node>> scenarios;

  while (!lex.check(token_type::eof))
  {
    auto tags = parse_tags(lex);
    if (lex.check(token_type::scenario))
    {
      const std::size_t line = lex.current().line;
      auto [key, name] = parse_keyword_and_name(lex);
      auto steps = parse_steps(lex);
      // TODO tags +description
      std::vector<std::string> tags{};
      std::vector<std::string> description{};

      scenarios.push_back(std::make_unique<cuke::ast::scenario_node>(
          std::move(key), std::move(name), lex.filepath(), line,
          std::move(steps), std::move(tags), std::move(description)));
    }
    else
    {
      // TODO proper error
      std::cout << "[Error] wrong token " << (int)lex.current().type << ' '
                << lex.current().value << std::endl;
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
  const std::size_t line = lex.current().line;
  auto [key, name] = parse_keyword_and_name(lex);
  auto description = parse_description(
      lex, token_type::scenario, token_type::scenario_outline, token_type::tag,
      token_type::background, token_type::eof);

  auto scenarios = parse_scenarios(lex);

  return cuke::ast::feature_node(std::move(key), std::move(name),
                                 lex.filepath(), line, std::move(scenarios),
                                 std::move(tags), std::move(description));
}

}  // namespace cwt::details

namespace cuke
{

// TODO
// struct file
// {
//   std::string path;
//   std::string content;
// };

// TODO
// [[nodiscard]] file read_file(std::string_view src)
// {
// TODO ...
// }

class parser
{
 public:
  const cuke::ast::gherkin_document& head() const noexcept { return m_head; }

  void parse_from_file(std::string_view filepath)
  {
    // TODO ...
  }

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