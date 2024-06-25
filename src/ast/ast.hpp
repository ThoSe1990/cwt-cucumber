#pragma once

#include "../lexer.hpp"
#include "../token.hpp"

#include <string>
#include <vector>

namespace cuke::ast::visitor
{
class node
{
 public:
  virtual ~node() = default;
  // virtual void visit(visitor& v) = 0; 

 private:
  struct
  {
    std::string file;
    std::size_t line;
  } m_location;
  std::string m_keyword;
  std::string m_name;
};
class gherkin_document : public node
{
 public:
 private:
};
class feature : public node
{
 public:
 private:
  std::vector<std::string> m_tags;
  std::vector<node> m_scenarios;
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

}  // namespace cuke::ast::visitor

namespace cuke::ast
{
enum class node_type
{
  gherkin_document = 0,
  feature,
  background,
  scenario,
  scenario_outline
};
struct info
{
  node_type type;
  struct
  {
    std::string file;
    std::size_t line;
  } location;
  std::string keyword;
  std::string value;
  std::string description;
};
struct step
{
  info infos;
  // TODO: datatable
  // TODO: doc string
};
struct node
{
  info infos;
  std::vector<node> children;
  // TODO tags
};

}  // namespace cuke::ast
