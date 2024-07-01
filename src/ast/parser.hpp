#pragma once

#include <ranges>
#include <string_view>

#include "ast.hpp"
#include "../token.hpp"
#include "../util.hpp"
#include "../table.hpp"

namespace cwt::details
{
[[nodiscard]] auto parse_keyword_and_name(lexer& lex)
{
  std::string key = create_string(lex.current().value);
  lex.advance();

  auto make_name = [](lexer& lex)
  {
    token begin = lex.current();
    lex.advance_to(token_type::linebreak, token_type::eof);
    token end = lex.previous();
    return create_string(begin, end);
  };

  std::string name = lex.current().type == token_type::linebreak
                         ? std::string("")
                         : make_name(lex);
  lex.advance();
  return std::make_pair(key, name);
}
template <typename... Ts>
[[nodiscard]] std::vector<std::string> parse_description(lexer& lex,
                                                         Ts&&... terminators)
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

[[nodiscard]] std::vector<std::string> parse_tags(lexer& lex)
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

[[nodiscard]] std::string trim(const std::string& str)
{
  auto is_space = [](char c)
  { return std::isspace(static_cast<unsigned char>(c)); };
  auto start = std::find_if_not(str.begin(), str.end(), is_space);
  auto end = std::find_if_not(str.rbegin(), str.rend(), is_space).base();
  return (start < end ? std::string(start, end) : "");
}

[[nodiscard]] std::vector<std::string> doc_string_to_vector(
    const std::string_view s, std::size_t lines_count)
{
  const std::size_t lines_count_wo_quotes = lines_count - 2;
  auto lines_view = s | std::ranges::views::split('\n');
  std::vector<std::string> lines;
  lines.reserve(lines_count_wo_quotes);
  for (auto&& line : lines_view | std::views::drop(1) |
                         std::views::take(lines_count_wo_quotes))
  {
    lines.push_back(trim(std::string(line.begin(), line.end())));
  }
  return lines;
}

[[nodiscard]] std::vector<std::string> parse_doc_string(lexer& lex)
{
  const std::size_t begin = lex.previous().line;
  if (lex.match(token_type::doc_string))
  {
    const std::size_t lines_count = lex.current().line - begin;
    std::string_view doc_string = lex.previous().value;
    return doc_string_to_vector(doc_string, lines_count);
  }
  else
  {
    return {};
  }
}
[[nodiscard]] cuke::value_array parse_row(lexer& lex)
{
  cuke::value_array v;
  while (!(lex.match(token_type::linebreak) || lex.match(token_type::eof)))
  {
    bool negative = lex.match(token_type::minus);
    v.push_back(token_to_value(lex.current(), negative));
    lex.advance();
    if (!lex.match(token_type::vertical))
    {
      v.clear();
      lex.error_at(lex.current(), "Expect '|' after value in data table");
      break;
    }
  }
  return v;
}
[[nodiscard]] cuke::table parse_table(lexer& lex)
{
  if (!lex.match(token_type::vertical))
  {
    return {};
  }
  cuke::table t(parse_row(lex));
  while (lex.match(token_type::vertical))
  {
    if (!t.append_row(parse_row(lex)) || lex.error())
    {
      lex.error_at(lex.current(), "Different row lengths in data table");
      return {};
    }
  }
  return t;
}
[[nodiscard]] std::vector<cuke::ast::step_node> parse_steps(lexer& lex)
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
      cuke::table data_table = parse_table(lex);

      steps.push_back(cuke::ast::step_node(
          std::move(key), std::move(name), lex.filepath(), line,
          std::move(doc_string), std::move(data_table)));

      lex.skip_linebreaks();
    }
    else
    {
      lex.error_at(lex.current(), "Expect Step, Scenario or Scenario Outline");
      return {};
    }
  } while (lex.is_none_of(token_type::scenario, token_type::scenario_outline,
                          token_type::tag, token_type::eof));
  return steps;
}
[[nodiscard]] std::vector<cuke::ast::example_node> parse_examples(lexer& lex)
{

  return {};
}
[[nodiscard]] std::unique_ptr<cuke::ast::scenario_outline_node>
make_scenario_outline(lexer& lex, std::vector<std::string>&& tags)
{
  const std::size_t line = lex.current().line;
  auto [key, name] = parse_keyword_and_name(lex);
  auto description = parse_description(lex, token_type::step, token_type::eof);
  auto steps = parse_steps(lex);  
  auto examples = parse_examples(lex);

  return std::make_unique<cuke::ast::scenario_outline_node>(
      std::move(key), std::move(name), lex.filepath(), line, std::move(steps),
      std::move(tags), std::move(description), std::move(examples));
}
[[nodiscard]] std::unique_ptr<cuke::ast::scenario_node> make_scenario(
    lexer& lex, std::vector<std::string>&& tags)
{
  const std::size_t line = lex.current().line;
  auto [key, name] = parse_keyword_and_name(lex);
  auto description = parse_description(lex, token_type::step, token_type::eof);
  auto steps = parse_steps(lex);
  return std::make_unique<cuke::ast::scenario_node>(
      std::move(key), std::move(name), lex.filepath(), line, std::move(steps),
      std::move(tags), std::move(description));
}
[[nodiscard]] std::vector<std::unique_ptr<cuke::ast::node>> parse_scenarios(
    lexer& lex)
{
  std::vector<std::unique_ptr<cuke::ast::node>> scenarios;

  while (!lex.check(token_type::eof))
  {
    auto tags = parse_tags(lex);
    if (lex.check(token_type::scenario))
    {
      scenarios.push_back(make_scenario(lex, std::move(tags)));
    }
    else if (lex.check(token_type::scenario_outline))
    {
      scenarios.push_back(make_scenario_outline(lex, std::move(tags)));
    }
    else
    {
      lex.error_at(lex.current(), "Expect Tags, Scenario or Scenario Outline");
      break;
    }
  }
  return std::move(scenarios);
}

[[nodiscard]] cuke::ast::feature_node parse_feature(lexer& lex)
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