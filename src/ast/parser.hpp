#pragma once

#include <memory>
#include <ranges>
#include <string_view>

#include "ast.hpp"
#include "../token.hpp"
#include "../util.hpp"
#include "../table.hpp"

namespace cwt::details
{

[[nodiscard]] static auto parse_keyword_and_name(lexer& lex)
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
  lex.skip_linebreaks();
  return std::make_pair(key, name);
}

template <typename... Ts>
[[nodiscard]] static std::vector<std::string> parse_description(
    lexer& lex, Ts&&... terminators)
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

[[nodiscard]] static std::vector<std::string> parse_tags(lexer& lex)
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

[[nodiscard]] static std::string trim(const std::string& str)
{
  auto is_space = [](char c)
  { return std::isspace(static_cast<unsigned char>(c)); };
  auto start = std::find_if_not(str.begin(), str.end(), is_space);
  auto end = std::find_if_not(str.rbegin(), str.rend(), is_space).base();
  return (start < end ? std::string(start, end) : "");
}

[[nodiscard]] static std::vector<std::string> doc_string_to_vector(
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

[[nodiscard]] static std::vector<std::string> parse_doc_string(lexer& lex)
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

[[nodiscard]] static std::size_t advance_to_cell_end(lexer& lex)
{
  std::size_t count = 0;
  while (!lex.check(token_type::vertical))
  {
    ++count;
    lex.advance();
    if (lex.check(token_type::eof, token_type::linebreak))
    {
      lex.error_at(lex.current(), "Expect '|' after value in cell");
      return 0;
    }
  }
  return count;
}

[[nodiscard]] static cuke::value parse_cell(lexer& lex)
{
  bool negative = lex.match(token_type::minus);
  token begin = lex.current();

  std::size_t count = advance_to_cell_end(lex);

  if (count == 1)
  {
    return cuke::value(token_to_value(lex.previous(), negative));
  }
  else if (count > 1)
  {
    return cuke::value(create_string(begin, lex.previous()));
  }
  else
  {
    lex.error_at(lex.current(), "Expect value in table cell");
    return {};
  }
}

[[nodiscard]] static cuke::value_array parse_row(lexer& lex)
{
  cuke::value_array v;
  while (!(lex.match(token_type::linebreak) || lex.match(token_type::eof)))
  {
    v.push_back(parse_cell(lex));
    if (!lex.match(token_type::vertical))
    {
      v.clear();
      lex.error_at(lex.current(), "Expect '|' after value in data table");
      break;
    }
  }
  return v;
}

[[nodiscard]] static cuke::table parse_table(lexer& lex)
{
  if (!lex.match(token_type::vertical))
  {
    return {};
  }
  cuke::table t(parse_row(lex));
  lex.skip_linebreaks();
  while (lex.match(token_type::vertical))
  {
    if (!t.append_row(parse_row(lex)) || lex.error())
    {
      lex.error_at(lex.current(), "Different row lengths in data table");
      return {};
    }
    lex.skip_linebreaks();
  }
  return t;
}

[[nodiscard]] static std::vector<cuke::ast::step_node> parse_steps(lexer& lex)
{
  using namespace cwt::details;
  std::vector<cuke::ast::step_node> steps;
  while (lex.check(token_type::step))
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
  return steps;
}

[[nodiscard]] static cuke::ast::example_node parse_example(
    lexer& lex, std::vector<std::string>&& tags)
{
  const std::size_t line = lex.current().line;
  auto [keyword, name] = parse_keyword_and_name(lex);
  auto description =
      parse_description(lex, token_type::vertical, token_type::eof);
  cuke::table t = parse_table(lex);
  return cuke::ast::example_node(cuke::ast::example_node(
      std::move(keyword), std::move(name), lex.filepath(), line,
      std::move(tags), std::move(description), std::move(t)));
}

[[nodiscard]] static std::unique_ptr<cuke::ast::scenario_outline_node>
make_scenario_outline(lexer& lex, std::vector<std::string>&& tags)
{
  const std::size_t line = lex.current().line;
  auto [key, name] = parse_keyword_and_name(lex);
  auto description = parse_description(lex, token_type::step, token_type::eof);
  auto steps = parse_steps(lex);

  return std::make_unique<cuke::ast::scenario_outline_node>(
      std::move(key), std::move(name), lex.filepath(), line, std::move(steps),
      std::move(tags), std::move(description));
}

[[nodiscard]] static std::unique_ptr<cuke::ast::scenario_node> make_scenario(
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

[[nodiscard]] static std::vector<std::unique_ptr<cuke::ast::node>>
parse_scenarios(lexer& lex)
{
  std::vector<std::unique_ptr<cuke::ast::node>> scenarios;

  while (!lex.error() && !lex.check(token_type::eof))
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
    else if (lex.check(token_type::examples) &&
             scenarios.back()->type() == cuke::ast::node_type::scenario_outline)
    {
      static_cast<cuke::ast::scenario_outline_node&>(*scenarios.back())
          .push_example(parse_example(lex, std::move(tags)));
    }
    else
    {
      lex.error_at(lex.current(), "Expect Tags, Scenario or Scenario Outline");
      break;
    }
    lex.skip_linebreaks();
  }
  return std::move(scenarios);
}

[[nodiscard]] static std::unique_ptr<cuke::ast::background_node>
parse_background(lexer& lex)
{
  if (lex.check(cwt::details::token_type::background))
  {
    const std::size_t line = lex.current().line;
    auto [key, name] = parse_keyword_and_name(lex);
    auto description =
        parse_description(lex, token_type::step, token_type::eof);
    auto steps = parse_steps(lex);
    return std::make_unique<cuke::ast::background_node>(
        std::move(key), std::move(name), lex.filepath(), line, std::move(steps),
        std::move(description));
  }
  return {};
}

[[nodiscard]] static cuke::ast::feature_node parse_feature(lexer& lex)
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
  lex.skip_linebreaks();
  auto background = parse_background(lex);
  auto scenarios = parse_scenarios(lex);

  return cuke::ast::feature_node(std::move(key), std::move(name),
                                 lex.filepath(), line, std::move(scenarios),
                                 std::move(background), std::move(tags),
                                 std::move(description));
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

template <typename T>
concept ScenarioVisitor =
    requires(T t, const ast::scenario_outline_node& scenario,
             const ast::scenario_node& scenario_outline) {
      t.visit(scenario);
      t.visit(scenario_outline);
    };

class parser
{
 public:
  [[nodiscard]] const cuke::ast::gherkin_document& head() const noexcept
  {
    return m_head;
  }
  [[nodiscard]] bool error() const noexcept { return m_error; }

  void parse_from_file(std::string_view filepath)
  {
    // TODO ...
  }

  void parse_script(std::string_view script)
  {
    m_error = false;
    using namespace cwt::details;
    lexer lex(script);
    lex.advance();
    lex.skip_linebreaks();
    m_head.set_feature(parse_feature(lex));
    if (lex.error())
    {
      m_error = true;
      m_head.clear();
    }
  }
  template <ScenarioVisitor Visitor>
  void for_each_scenario(Visitor& visitor)
  {
    for (const auto& n : m_head.feature().scenarios())
    {
      if (n->type() == ast::node_type::scenario)
      {
        visitor.visit(static_cast<ast::scenario_node&>(*n));
      }
      else if (n->type() == ast::node_type::scenario_outline)
      {
        visitor.visit(static_cast<ast::scenario_outline_node&>(*n));
      }
    }
  }

 private:
  cuke::ast::gherkin_document m_head;
  bool m_error{false};
};

}  // namespace cuke
