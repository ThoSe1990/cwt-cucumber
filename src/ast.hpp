#pragma once

#include "step.hpp"
#include "table.hpp"
#include "value.hpp"
#include "step_finder.hpp"
#include "registry.hpp"

#include <cstddef>
#include <memory>
#include <nlohmann/detail/value_t.hpp>
#include <optional>
#include <string>
#include <vector>

namespace cuke::ast
{
enum class node_type
{
  gherkin_document = 0,
  feature,
  background,
  scenario,
  scenario_outline,
  step,
  rule,
  example
};

class node
{
 public:
  node() = default;
  node(const std::string& keyword, const std::string& name, std::size_t line,
       const std::string& file = "")
      : m_keyword(keyword), m_name(name), m_location{line, file}
  {
  }
  node(std::string&& keyword, std::string&& name, std::size_t line,
       const std::string& file = "")
      : m_keyword(std::move(keyword)),
        m_name(std::move(name)),
        m_location{line, file}
  {
  }

  virtual ~node() = default;
  virtual node_type type() const noexcept = 0;

  [[nodiscard]] const std::string& name() const { return m_name; }
  [[nodiscard]] const std::string& keyword() const { return m_keyword; }
  [[nodiscard]] const std::string& file() const { return m_location.file; }
  [[nodiscard]] std::size_t line() const { return m_location.line; }

 private:
  std::string m_keyword{""};
  std::string m_name{""};
  struct
  {
    std::size_t line{0};
    std::string file{""};
  } m_location;
};

class step_node : public node
{
 public:
  step_node(const std::string& key, const std::string& name,
            const std::string& file, std::size_t line)
      : node(key, name, line, file), m_has_step_definition(false)
  {
  }
  step_node(const std::string& key, const std::string& name,
            const std::string& file, std::size_t line,
            const std::vector<std::string>& doc_string,
            const cuke::table& data_table, const value_array& values,
            const internal::step_definition* step_definition)
      : node(key, name, line, file),
        m_doc_string(doc_string),
        m_table(data_table),
        m_values(values),
        m_step_definition(step_definition),
        m_has_step_definition(step_definition != nullptr)
  {
  }
  step_node(std::string&& key, std::string&& name, const std::string& file,
            std::size_t line, std::vector<std::string>&& doc_string,
            cuke::table&& data_table)
      : node(std::move(key), std::move(name), line, file),
        m_doc_string(std::move(doc_string)),
        m_table(std::move(data_table))
  {
    internal::step_finder finder(this->name());
    auto it = finder.find(cuke::registry().steps().begin(),
                          cuke::registry().steps().end());
    m_has_step_definition = it != cuke::registry().steps().end();
    if (m_has_step_definition)
    {
      m_values = finder.values();
      m_step_definition = &*it;
    }
  }
  [[nodiscard]] node_type type() const noexcept override
  {
    return node_type::step;
  }
  [[nodiscard]] const std::vector<std::string>& doc_string() const noexcept
  {
    return m_doc_string;
  }
  [[nodiscard]] const value_array& values() const noexcept { return m_values; }
  [[nodiscard]] const cuke::table& data_table() const noexcept
  {
    return m_table;
  }
  [[nodiscard]] bool has_step_definition() const noexcept
  {
    return m_has_step_definition;
  }
  void call() const noexcept
  {
    if (m_step_definition)
    {
      m_step_definition->call(m_values, m_doc_string, m_table);
    }
    // TODO: else ?
  }

  std::string source_location_definition() const noexcept
  {
    if (m_step_definition) [[likely]]
    {
      return m_step_definition->source_location();
    }
    else [[unlikely]]
    {
      return {};
    }
  }

  // FIXME: remove the non const version after creating
  // an executable tree or stack
  [[nodiscard]] cuke::table& data_table() noexcept { return m_table; }

 private:
  std::vector<std::string> m_doc_string;
  cuke::table m_table;
  bool m_has_step_definition;
  // TODO: make this a unique ptr?
  const internal::step_definition* m_step_definition;
  value_array m_values;
};

class background_node : public node
{
 public:
  background_node(std::string&& key, std::string&& name,
                  const std::string& file, std::size_t line,
                  std::vector<step_node>&& steps,
                  std::vector<std::string>&& description)
      : node(std::move(key), std::move(name), line, file),
        m_steps(std::move(steps)),
        m_description(std::move(description))
  {
  }
  [[nodiscard]] node_type type() const noexcept override
  {
    return node_type::background;
  }

  const std::vector<step_node> steps() const noexcept { return m_steps; }
  const std::vector<std::string>& description() const noexcept
  {
    return m_description;
  }

 private:
  std::vector<step_node> m_steps;
  std::vector<std::string> m_description;
};
class rule_node : public node
{
 public:
  rule_node(std::string&& key, std::string&& name, const std::string& file,
            std::size_t line, std::vector<std::string>&& description)
      : node(std::move(key), std::move(name), line, file),
        m_description(std::move(description))
  {
  }

  [[nodiscard]] node_type type() const noexcept override
  {
    return node_type::rule;
  }
  [[nodiscard]] const std::vector<std::string>& description() const noexcept
  {
    return m_description;
  }

 private:
  std::vector<std::string> m_description;
};

class example_node;
class scenario_outline_node;

class scenario_node : public node
{
 public:
  scenario_node(std::string&& key, std::string&& name, const std::string& file,
                std::size_t line, std::vector<step_node>&& steps,
                std::vector<std::string>&& tags,
                std::vector<std::string>&& description,
                const std::optional<rule_node>& rule,
                const background_node* background)
      : node(std::move(key), std::move(name), line, file),
        m_steps(std::move(steps)),
        m_tags(std::move(tags)),
        m_description(std::move(description)),
        m_rule(rule),
        m_background(background)
  {
  }
  scenario_node(const scenario_outline_node& scenario_outline,
                const example_node& examples, std::size_t row,
                const background_node* background);
  [[nodiscard]] node_type type() const noexcept override
  {
    return node_type::scenario;
  }
  [[nodiscard]] const std::optional<rule_node>& rule() const noexcept
  {
    return m_rule;
  }
  [[nodiscard]] const std::vector<step_node>& steps() const noexcept
  {
    return m_steps;
  }
  [[nodiscard]] const std::vector<std::string>& tags() const noexcept
  {
    return m_tags;
  }
  [[nodiscard]] const std::vector<std::string>& description() const noexcept
  {
    return m_description;
  }
  [[nodiscard]] bool has_background() const noexcept
  {
    return m_background != nullptr;
  }
  [[nodiscard]] const background_node& background() const noexcept
  {
    return *m_background;
  }

 private:
  std::vector<step_node> m_steps;
  std::vector<std::string> m_tags;
  std::vector<std::string> m_description;
  std::optional<rule_node> m_rule;
  const background_node* m_background;
};

class example_node : public node
{
 public:
  example_node(std::string&& key, std::string&& name, const std::string& file,
               std::size_t line, std::vector<std::string>&& tags,
               std::vector<std::string>&& description, cuke::table&& table,
               std::size_t line_table_begin)
      : node(std::move(key), std::move(name), line, file),
        m_tags(std::move(tags)),
        m_description(std::move(description)),
        m_table(std::move(table)),
        m_line_table_begin(line_table_begin)
  {
  }

  [[nodiscard]] node_type type() const noexcept override
  {
    return node_type::example;
  }
  [[nodiscard]] const std::vector<std::string>& tags() const noexcept
  {
    return m_tags;
  }
  [[nodiscard]] const std::vector<std::string>& description() const noexcept
  {
    return m_description;
  }
  [[nodiscard]] const cuke::table& table() const noexcept { return m_table; }
  // FIXME: with this implementation an empty line in a table
  // does not consider running a specific line anymore. it does not take
  // the empty line into acocount since in the test_runner this value is just
  // incremented.
  [[nodiscard]] std::size_t line_table_begin() const noexcept
  {
    return m_line_table_begin;
  }

 private:
  std::vector<std::string> m_tags;
  std::vector<std::string> m_description;
  cuke::table m_table;
  std::size_t m_line_table_begin;
};

class scenario_outline_node : public node
{
 public:
  scenario_outline_node(std::string&& key, std::string&& name,
                        const std::string& file, std::size_t line,
                        std::vector<step_node>&& steps,
                        std::vector<std::string>&& tags,
                        std::vector<std::string>&& description,
                        const std::optional<rule_node>& rule)
      : node(std::move(key), std::move(name), line, file),
        m_steps(std::move(steps)),
        m_tags(std::move(tags)),
        m_description(std::move(description)),
        m_rule(rule)
  {
  }
  const std::vector<step_node>& steps() const noexcept { return m_steps; }
  const std::vector<std::string>& tags() const noexcept { return m_tags; }
  const std::vector<std::string>& description() const noexcept
  {
    return m_description;
  }
  [[nodiscard]] node_type type() const noexcept override
  {
    return node_type::scenario_outline;
  }
  [[nodiscard]] const std::vector<example_node>& examples() const noexcept
  {
    return m_examples;
  }
  [[nodiscard]] const std::optional<rule_node>& rule() const noexcept
  {
    return m_rule;
  }
  [[nodiscard]] std::size_t scenarios_count() const noexcept
  {
    return m_concrete_scenarios.size();
  }
  [[nodiscard]] const std::vector<scenario_node>& concrete_scenarios()
      const noexcept
  {
    return m_concrete_scenarios;
  }
  [[nodiscard]] const scenario_node& scenario(std::size_t idx) const noexcept
  {
    return m_concrete_scenarios[idx];
  }
  void push_example(example_node&& example, const background_node* background)
  {
    m_examples.push_back(std::move(example));
    const auto& current = m_examples.back();
    for (std::size_t i = 1; i < current.table().row_count(); ++i)
    {
      m_concrete_scenarios.push_back(
          scenario_node(*this, current, i, background));
    }
  }

 private:
  std::vector<step_node> m_steps;
  std::vector<std::string> m_tags;
  std::vector<std::string> m_description;
  std::vector<example_node> m_examples;
  std::optional<rule_node> m_rule;
  std::vector<scenario_node> m_concrete_scenarios;
};

class feature_node : public node
{
 public:
  feature_node() = default;
  feature_node(std::string&& key, std::string&& name, const std::string& file,
               std::size_t line, std::vector<std::unique_ptr<node>>&& scenarios,
               std::unique_ptr<background_node> background,
               std::vector<std::string>&& tags,
               std::vector<std::string>&& description)
      : node(std::move(key), std::move(name), line, file),
        m_scenarios(std::move(scenarios)),
        m_background(std::move(background)),
        m_tags(std::move(tags)),
        m_description(std::move(description))
  {
  }
  [[nodiscard]] node_type type() const noexcept override
  {
    return node_type::feature;
  }
  [[nodiscard]] const std::vector<std::string>& tags() const noexcept
  {
    return m_tags;
  }
  [[nodiscard]] const std::vector<std::string>& description() const noexcept
  {
    return m_description;
  }
  [[nodiscard]] const std::vector<std::unique_ptr<node>>& scenarios()
      const noexcept
  {
    return m_scenarios;
  }
  [[nodiscard]] bool has_background() const noexcept
  {
    return m_background != nullptr;
  }
  [[nodiscard]] const background_node& background() const noexcept
  {
    return *m_background;
  }

 private:
  std::vector<std::unique_ptr<node>> m_scenarios;
  std::unique_ptr<background_node> m_background;
  std::vector<std::string> m_tags;
  std::vector<std::string> m_description;
};

class gherkin_document
{
 public:
  [[nodiscard]] const feature_node& feature() const { return *m_feature; }
  void clear() { m_feature.reset(); }

  template <typename... Args>
  void set_feature(Args&&... args)
  {
    m_feature = std::make_unique<feature_node>(std::forward<Args>(args)...);
  }

 private:
  std::unique_ptr<feature_node> m_feature;
};

}  // namespace cuke::ast
