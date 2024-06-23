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

class parser
{
 public:
  parser(std::string_view src) : m_lexer(src)
  {
    m_head.infos.type = cuke::ast::node_type::gherkin_document;
  }
  cuke::ast::node& head() noexcept { return m_head; }
  void parse()
  {
    m_lexer.advance();
    m_lexer.skip_linebreaks();
    cuke::ast::node node;
    node.infos.type = ast::node_type::feature;
    switch (m_lexer.current().type)
    {
      case cwt::details::token_type::tag:
      {
        // TODO
        // push tags to feature node
        // node.tags.push(...);
      }
      break;
      case cwt::details::token_type::feature:
      {
        m_lexer.advance();
        feature(node);
      }
      break;
      default:
      {
        m_lexer.error_at(m_lexer.current(), "Expect FeatureLine");
        return;
      }
    }
    m_head.children.push_back(node);
  }

 private:
  void scenario(cuke::ast::node& scenario_node)
  {
    m_lexer.skip_linebreaks();
    // cuke::ast::node node;
    scenario_node.infos.type = node_type::scenario;
    do
    {
      if (m_lexer.match(cwt::details::token_type::step))
      {
        // TODO
        // step s(this);
        // s.compile();
        // node.steps ( ??? ) ;
      }
      else
      {
        m_lexer.error_at(m_lexer.current(), "Expect StepLine");
        return;
      }
      m_lexer.skip_linebreaks();
    } while (m_lexer.is_none_of(cwt::details::token_type::scenario,
                                cwt::details::token_type::scenario_outline,
                                cwt::details::token_type::tag,
                                cwt::details::token_type::eof));

    // scenario_node.children.push_back( step node? )
  }
  void feature(cuke::ast::node& feature_node)
  {
    while (!m_lexer.check(cwt::details::token_type::eof))
    {
      m_lexer.skip_linebreaks();
      cuke::ast::node node;
      switch (m_lexer.current().type)
      {
        case cwt::details::token_type::tag:
        {
          // TODO
          // push tags to feature node
          // node.tags.push(...);
        }
        break;
        case cwt::details::token_type::scenario:
        {
          m_lexer.advance();
          scenario(node);
        }
        break;
        default:
        {
          // TODO error
          break;
        }
      }

      feature_node.children.push_back(node);
    }
  }

 private:
  cwt::details::lexer m_lexer;
  cuke::ast::node m_head;
};
}  // namespace cuke::ast
