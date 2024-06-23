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
