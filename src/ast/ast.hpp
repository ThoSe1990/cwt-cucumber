#pragma once

#include "../lexer.hpp"
#include "../token.hpp"
#include "../table.hpp"

#include <string>
#include <vector>

namespace cuke::ast
{
enum class node_type
{
  gherkin_document = 0,
  tag,
  feature,
  background,
  scenario,
  scenario_outline,
  step,
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
  // virtual void visit(visitor& v) = 0;

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

class background_node : public node
{
 public:
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

  const std::vector<std::string>& doc_string() const noexcept
  {
    return m_doc_string;
  }

  const cuke::table& data_table() const noexcept { return m_table; }

 private:
  std::vector<std::string> m_doc_string;
  cuke::table m_table;
};

class scenario_node : public node
{
 public:
  scenario_node(std::string&& key, std::string&& name, const std::string& file,
                std::size_t line, std::vector<step_node>&& steps,
                std::vector<std::string>&& tags,
                std::vector<std::string>&& description)
      : node(std::move(key), std::move(name), line, file),
        m_steps(std::move(steps)),
        m_tags(std::move(tags)),
        m_description(std::move(description))
  {
  }
  [[nodiscard]] node_type type() const noexcept override
  {
    return node_type::scenario;
  }

  const std::vector<step_node> steps() const noexcept { return m_steps; }
  const std::vector<std::string>& tags() const noexcept { return m_tags; }
  const std::vector<std::string>& description() const noexcept
  {
    return m_description;
  }

 private:
  std::vector<step_node> m_steps;
  std::vector<std::string> m_tags;
  std::vector<std::string> m_description;
};

class example_node : public node
{
 public:
  [[nodiscard]] node_type type() const noexcept override
  {
    return node_type::example;
  }
  [[nodiscard]] const cuke::table& table() const noexcept { return m_table; }

 private:
  std::string m_description;
  cuke::table m_table;
};

class scenario_outline_node : public scenario_node
{
 public:
  scenario_outline_node(std::string&& key, std::string&& name,
                        const std::string& file, std::size_t line,
                        std::vector<step_node>&& steps,
                        std::vector<std::string>&& tags,
                        std::vector<std::string>&& description,
                        std::vector<example_node>&& examples)
      : scenario_node(std::move(key), std::move(name), file, line,
                      std::move(steps), std::move(tags),
                      std::move(description)),
        m_examples(std::move(examples))
  {
  }
  [[nodiscard]] node_type type() const noexcept override
  {
    return node_type::scenario_outline;
  }
  [[nodiscard]] const std::vector<example_node>& examples() const noexcept
  {
    return m_examples;
  }

 private:
  std::vector<example_node> m_examples;
};

class feature_node : public node
{
 public:
  feature_node() = default;
  feature_node(std::string&& key, std::string&& name, const std::string& file,
               std::size_t line, std::vector<std::unique_ptr<node>>&& scenarios,
               std::vector<std::string>&& tags,
               std::vector<std::string>&& description)
      : node(std::move(key), std::move(name), line, file),
        m_scenarios(std::move(scenarios)),
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
  [[nodiscard]] const background_node& background() const noexcept
  {
    return *m_background;
  }

 private:
  std::vector<std::unique_ptr<node>> m_scenarios;
  std::vector<std::string> m_tags;
  std::vector<std::string> m_description;
  std::unique_ptr<background_node> m_background;
};

class gherkin_document
{
 public:
  [[nodiscard]] const feature_node& feature() const { return *m_feature; }
  void clear() { m_feature.reset(); }

  template <typename... Args>
  void make_feature(Args&&... args)
  {
    m_feature = std::make_unique<feature_node>(std::forward<Args>(args)...);
  }

 private:
  std::unique_ptr<feature_node> m_feature;
};

}  // namespace cuke::ast
