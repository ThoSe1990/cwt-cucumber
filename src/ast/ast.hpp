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
  node(node_type type, const std::string& keyword = "",
       const std::string& name = "", const std::string& file = "",
       std::size_t line = 0)
      : m_type(type), m_location{file, line}, m_keyword(keyword), m_name(name)
  {
  }

  virtual ~node() = default;
  // virtual void visit(visitor& v) = 0;

  node_type type() const { return m_type; }
  const std::string& name() const { return m_name; }
  const std::string& keyword() const { return m_keyword; }
  const std::string& file() const { return m_location.file; }
  std::size_t line() const { return m_location.line; }

 private:
  node_type m_type;
  struct
  {
    std::string file{""};
    std::size_t line{0};
  } m_location;
  std::string m_keyword{""};
  std::string m_name{""};
};

class feature_node : public node
{
 public:
  feature_node(const std::string& key, const std::string& name,
               const std::vector<std::string>& tags,
               const std::vector<std::string>& description = {})
      : node(node_type::feature, key, name),
        m_tags(tags),
        m_description(description)
  {
  }
  const std::vector<std::string>& tags() const noexcept { return m_tags; }
  const std::vector<std::string>& description() const noexcept
  {
    return m_description;
  }

 private:
  std::vector<std::string> m_tags;
  std::vector<std::string> m_description;
  std::vector<std::unique_ptr<node>> m_scenarios;
};
class gherkin_document : public node
{
 public:
  gherkin_document() : node{node_type::gherkin_document} {}
  const feature_node& feature() const { return *m_feature; }
  template <typename... Args>
  void make_feature(Args&&... args)
  {
    m_feature = std::make_unique<feature_node>(std::forward<Args>(args)...);
  }

 private:
  std::unique_ptr<feature_node> m_feature;
};
class background : public node
{
 public:
};
class scenario : public node
{
 public:
 private:
  std::vector<std::string> m_tags;
};
class scenario_outline : public node
{
 public:
 private:
  std::vector<std::string> m_tags;
};
class step : public node
{
 public:
};

}  // namespace cuke::ast
