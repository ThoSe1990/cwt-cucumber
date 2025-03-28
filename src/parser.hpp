#pragma once

#include <memory>
#include <optional>
#include <ranges>
#include <string_view>

#include "ast.hpp"
#include "token.hpp"
#include "util.hpp"
#include "table.hpp"
#include "options.hpp"
#include "lexer.hpp"

namespace cuke::internal
{

[[nodiscard]] static auto parse_keyword_and_name(lexer& lex, bool remove_colon)
{
  std::string key = create_string(lex.current().value, remove_colon ? 1 : 0);
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
    const std::string_view s)
{
  auto lines_view = s | std::ranges::views::split('\n');
  std::vector<std::string> lines;
  for (auto&& line : lines_view | std::views::drop(1))
  {
    lines.push_back(trim(std::string(line.begin(), line.end())));
  }
  lines.pop_back();
  return lines;
}

[[nodiscard]] static std::vector<std::string> parse_doc_string(lexer& lex)
{
  if (lex.match(token_type::doc_string))
  {
    return doc_string_to_vector(lex.previous().value);
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

[[nodiscard]] static cuke::value parse_cell(lexer& lex,
                                            bool remove_quotes_from_strings)
{
  token begin = lex.current();

  std::size_t count = advance_to_cell_end(lex);

  if (count == 0)
  {
    lex.error_at(lex.current(), "Expect value in table cell");
    return {};
  }

  cuke::value v(create_string(begin, lex.previous()));
  if (remove_quotes_from_strings)
  {
    v.emplace_or_replace(remove_quotes(v.to_string()));
  }
  return v;
}

[[nodiscard]] static cuke::value_array parse_row(
    lexer& lex, bool remove_quotes_from_strings)
{
  cuke::value_array v;
  while (!(lex.match(token_type::linebreak) || lex.match(token_type::eof)))
  {
    v.push_back(parse_cell(lex, remove_quotes_from_strings));
    if (!lex.match(token_type::vertical))
    {
      v.clear();
      lex.error_at(lex.current(), "Expect '|' after value in data table");
      break;
    }
  }
  return v;
}

[[nodiscard]] static std::pair<cuke::table, std::vector<std::size_t>>
parse_table(lexer& lex, bool remove_quotes_from_strings)
{
  if (!lex.match(token_type::vertical))
  {
    return {};
  }
  std::vector<std::size_t> lines;
  lines.push_back(lex.current().line);
  cuke::table t(parse_row(lex, remove_quotes_from_strings));
  lex.skip_linebreaks();
  while (lex.match(token_type::vertical))
  {
    lines.push_back(lex.current().line);
    if (!t.append_row(parse_row(lex, remove_quotes_from_strings)) ||
        lex.error())
    {
      lex.error_at(lex.current(), "Different row lengths in data table");
      return {};
    }
    lex.skip_linebreaks();
  }
  return std::make_pair(std::move(t), std::move(lines));
}

[[nodiscard]] static std::vector<cuke::ast::step_node> parse_steps(lexer& lex)
{
  using namespace cuke::internal;
  std::vector<cuke::ast::step_node> steps;
  while (lex.check(token_type::step))
  {
    const std::size_t line = lex.current().line;
    auto [key, name] = parse_keyword_and_name(lex, false);
    std::vector<std::string> doc_string = parse_doc_string(lex);
    auto [data_table, line_table_begin] = parse_table(lex, true);

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
  auto [keyword, name] = parse_keyword_and_name(lex, true);
  auto description =
      parse_description(lex, token_type::vertical, token_type::eof);
  auto [t, lines_from_table_rows] = parse_table(lex, false);
  return cuke::ast::example_node(std::move(keyword), std::move(name),
                                 lex.filepath(), line, std::move(tags),
                                 std::move(description), std::move(t),
                                 std::move(lines_from_table_rows));
}

[[nodiscard]] static std::unique_ptr<cuke::ast::scenario_outline_node>
make_scenario_outline(lexer& lex, std::vector<std::string>&& tags,
                      const std::optional<cuke::ast::rule_node>& rule)
{
  const std::size_t line = lex.current().line;
  auto [key, name] = parse_keyword_and_name(lex, true);
  auto description = parse_description(lex, token_type::step, token_type::eof);
  auto steps = parse_steps(lex);

  return std::make_unique<cuke::ast::scenario_outline_node>(
      std::move(key), std::move(name), lex.filepath(), line, std::move(steps),
      std::move(tags), std::move(description), rule);
}

[[nodiscard]] static std::unique_ptr<cuke::ast::scenario_node> make_scenario(
    lexer& lex, std::vector<std::string>&& tags,
    const std::optional<cuke::ast::rule_node>& rule,
    const ast::background_node* background)
{
  const std::size_t line = lex.current().line;
  auto [key, name] = parse_keyword_and_name(lex, true);
  auto description = parse_description(lex, token_type::step, token_type::eof);
  auto steps = parse_steps(lex);
  return std::make_unique<cuke::ast::scenario_node>(
      std::move(key), std::move(name), lex.filepath(), line, std::move(steps),
      std::move(tags), std::move(description), rule, background);
}

[[nodiscard]] static std::optional<cuke::ast::rule_node> parse_rule(lexer& lex)
{
  if (lex.check(token_type::rule))
  {
    const std::size_t line = lex.current().line;
    auto [key, name] = parse_keyword_and_name(lex, true);
    auto description = parse_description(lex, token_type::scenario_outline,
                                         token_type::scenario, token_type::tag,
                                         token_type::eof);

    return cuke::ast::rule_node(std::move(key), std::move(name), lex.filepath(),
                                line, std::move(description));
  }

  return std::nullopt;
}

[[nodiscard]] static std::vector<std::unique_ptr<cuke::ast::node>>
parse_scenarios(lexer& lex, const std::vector<std::string>& feature_tags,
                const ast::background_node* background)
{
  std::vector<std::unique_ptr<cuke::ast::node>> scenarios;
  std::optional<cuke::ast::rule_node> current_rule = std::nullopt;

  while (!lex.error() && !lex.check(token_type::eof))
  {
    current_rule = parse_rule(lex);
    auto tags = parse_tags(lex);
    for (const auto& t : feature_tags)
    {
      if (std::find(tags.begin(), tags.end(), t) == tags.end())
      {
        tags.push_back(t);
      }
    }

    if (lex.check(token_type::scenario))
    {
      scenarios.push_back(
          make_scenario(lex, std::move(tags), current_rule, background));
    }
    else if (lex.check(token_type::scenario_outline))
    {
      scenarios.push_back(
          make_scenario_outline(lex, std::move(tags), current_rule));
    }
    else if (lex.check(token_type::examples) &&
             scenarios.back()->type() == cuke::ast::node_type::scenario_outline)
    {
      static_cast<cuke::ast::scenario_outline_node&>(*scenarios.back())
          .push_example(parse_example(lex, std::move(tags)), background);
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
  if (lex.check(cuke::internal::token_type::background))
  {
    const std::size_t line = lex.current().line;
    auto [key, name] = parse_keyword_and_name(lex, true);
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
  if (!lex.check(cuke::internal::token_type::feature))
  {
    lex.error_at(lex.current(), "Expect FeatureLine");
    return cuke::ast::feature_node();
  }
  const std::size_t line = lex.current().line;
  auto [key, name] = parse_keyword_and_name(lex, true);
  auto description = parse_description(
      lex, token_type::scenario, token_type::scenario_outline, token_type::tag,
      token_type::background, token_type::rule, token_type::eof);
  lex.skip_linebreaks();
  auto background = parse_background(lex);
  auto scenarios = parse_scenarios(lex, tags, background.get());

  return cuke::ast::feature_node(std::move(key), std::move(name),
                                 lex.filepath(), line, std::move(scenarios),
                                 std::move(background), std::move(tags),
                                 std::move(description));
}

}  // namespace cuke::internal

namespace cuke
{

template <typename T>
concept CukeVisitor = requires(
    T t, const ast::feature_node& feature, const ast::scenario_node& scenario,
    const ast::scenario_outline_node& scenario_outline) {
  t.visit(feature);
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

  void parse_from_file(const feature_file& file) { parse_from_file(file.path); }

  void parse_from_file(std::string_view filepath)
  {
    const std::string script = internal::read_file(filepath);
    if (script.empty())
    {
      println(internal::color::red, "Error: File not found '", filepath, "'");
      return;
    }
    parse_impl(script, filepath);
  }

  void parse_script(std::string_view script)
  {
    parse_impl(script, "<no file>");
  }

  const ast::scenario_node* get_scenario_from_line(std::size_t line) const
  {
    for (const auto& n : m_head.feature().scenarios())
    {
      if (n->type() == ast::node_type::scenario && n->line() == line)
      {
        return static_cast<const ast::scenario_node*>(n.get());
      }
      else if (n->type() == ast::node_type::scenario_outline)
      {
        for (const auto& scenario :
             static_cast<const ast::scenario_outline_node*>(n.get())
                 ->concrete_scenarios())
        {
          if (scenario.line() == line)
          {
            return &scenario;
          }
        }
      }
    }
    return nullptr;
  }

  template <CukeVisitor Visitor>
  void for_each_scenario(Visitor& visitor)
  {
    visitor.visit(m_head.feature());
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
  void parse_impl(std::string_view script, std::string_view filename)
  {
    using namespace cuke::internal;
    m_error = false;
    lexer lex(script, filename);
    lex.advance();
    lex.skip_linebreaks();
    m_head.set_feature(parse_feature(lex));
    if (lex.error())
    {
      println(internal::color::red, "Error while parsing script");
      m_error = true;
      m_head.clear();
    }
  }

 private:
  cuke::ast::gherkin_document m_head;
  bool m_error{false};
};

}  // namespace cuke
