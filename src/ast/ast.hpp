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
  virtual ~node() = default;
  // virtual void visit(visitor& v) = 0; 

//  private:
  struct
  {
    std::string file;
    std::size_t line;
  } m_location;
  std::string m_keyword;
  std::string m_name;
};

class feature_node : public node
{
 public:
  feature_node(const std::string& key, const std::string& name) 
  {
    m_keyword = key;
    m_name = name;
  }

  const std::string& name() const { return m_name; }
  const std::string& keyword() const { return m_keyword; }
 private:
  std::vector<std::string> m_tags;
  std::vector<node> m_scenarios;
};
class gherkin_document : public node
{
 public:
  node_type type() const { return m_type; }
  const feature_node& feature() const { return *m_feature; }
  
//  private:
 node_type m_type;
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

