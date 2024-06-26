#pragma once

#include "../lexer.hpp"
#include "../token.hpp"

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
  scenario_outline
};

class node
{
 public:
  node() = default;
  node(node_type type, std::string&& keyword, std::string&& name,
       std::size_t line, const std::string& file = "")
      : m_type(type),
        m_keyword(std::move(keyword)),
        m_name(std::move(name)),
        m_location{line, file}
  {
  }

  virtual ~node() = default;
  // virtual void visit(visitor& v) = 0;

  [[nodiscard]] node_type type() const { return m_type; }
  [[nodiscard]] const std::string& name() const { return m_name; }
  [[nodiscard]] const std::string& keyword() const { return m_keyword; }
  [[nodiscard]] const std::string& file() const { return m_location.file; }
  [[nodiscard]] std::size_t line() const { return m_location.line; }

 private:
  node_type m_type;
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

class step : public node
{
 public:
};

class scenario_node : public node
{
 public:
  // scenario_node() : m_description("") {}

 private:
  std::vector<std::string> m_tags;
  std::vector<std::string> m_description;
  // ... steps
};

class scenario_outline : public node
{
 public:
 private:
  std::vector<std::string> m_tags;
};

class feature_node : public node
{
 public:
  feature_node() = default;
  feature_node(std::string&& key, std::string&& name, const std::string& file,
               std::size_t line, std::vector<std::unique_ptr<node>>&& scenarios,
               std::vector<std::string>&& tags,
               std::vector<std::string>&& description)
      : node(node_type::feature, std::move(key), std::move(name), line, file),
        m_scenarios(std::move(scenarios)),
        m_tags(tags),
        m_description(description)
  {
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
