#pragma once

#include "table.hpp"

#include <cstddef>
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
  step_node(std::string&& key, std::string&& name, const std::string& file,
            std::size_t line, std::vector<std::string>&& doc_string,
            cuke::table&& data_table)
      : node(std::move(key), std::move(name), line, file),
        m_doc_string(std::move(doc_string)),
        m_table(std::move(data_table))
  {
  }
  [[nodiscard]] node_type type() const noexcept override
  {
    return node_type::step;
  }

  [[nodiscard]] const std::vector<std::string>& doc_string() const noexcept
  {
    return m_doc_string;
  }

  [[nodiscard]] const cuke::table& data_table() const noexcept
  {
    return m_table;
  }

  template <typename Callback>
  void if_has_table_do(Callback&& callback) const noexcept
  {
    if (!m_table.empty())
    {
      callback(m_table);
    }
  }

  template <typename Callback>
  void if_has_doc_string_do(Callback&& callback) const noexcept
  {
    if (m_doc_string.size() > 0)
    {
      callback(m_doc_string);
    }
  }

 private:
  std::vector<std::string> m_doc_string;
  cuke::table m_table;
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
                std::size_t line,
                std::vector<std::string>&& description)
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
class scenario_node : public node
{
 public:
  scenario_node(std::string&& key, std::string&& name, const std::string& file,
                std::size_t line, std::vector<step_node>&& steps,
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
  [[nodiscard]] node_type type() const noexcept override
  {
    return node_type::scenario;
  }
  [[nodiscard]] const std::optional<rule_node>& rule() const noexcept 
  {
    return m_rule; 
  }
  [[nodiscard]] const std::vector<step_node> steps() const noexcept { return m_steps; }
  [[nodiscard]] const std::vector<std::string>& tags() const noexcept { return m_tags; }
  [[nodiscard]] const std::vector<std::string>& description() const noexcept
  {
    return m_description;
  }

 private:
  std::vector<step_node> m_steps;
  std::vector<std::string> m_tags;
  std::vector<std::string> m_description;
  std::optional<rule_node> m_rule;
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
  const std::vector<step_node> steps() const noexcept { return m_steps; }
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
  void push_example(example_node&& example)
  {
    m_examples.push_back(std::move(example));
  }

 private:
  std::vector<step_node> m_steps;
  std::vector<std::string> m_tags;
  std::vector<std::string> m_description;
  std::vector<example_node> m_examples;
  std::optional<rule_node> m_rule;
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
