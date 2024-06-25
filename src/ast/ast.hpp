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
struct node
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
  std::vector<node> children;
  std::vector<node> tags; 
};


}  // namespace cuke::ast
