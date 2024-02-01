
#include "cucumber.hpp"
#include "feature.hpp"

namespace cwt::details::compiler
{
cucumber::cucumber(std::string_view source) : compiler(source, "") { init(); }
cucumber::cucumber(std::string_view source, std::string_view filename)
    : compiler(source, filename)
{
  init();
}
function cucumber::make_function() noexcept
{
  return std::make_unique<chunk>(get_chunk());
}
void cucumber::init()
{
  m_parser->advance();
  m_parser->skip_linebreaks();
}

bool compiler::tags_valid()
{
  return m_tag_expression->evaluate(m_tags.size(), m_tags.rbegin());
}

void compiler::read_tags()
{
  m_tags.clear();
  while (m_parser->check(token_type::tag))
  {
    m_tags.push_back(std::string(m_parser->current().value));
    m_parser->advance();
  }
}

void cucumber::compile()
{
  do
  {
    m_parser->skip_linebreaks();
    switch (m_parser->current().type)
    {
      case token_type::tag:
      {
        read_tags();
      }
      break;
      case token_type::feature:
      {
        compile_feature();
      }
      break;
      default:
      {
        m_parser->error_at(m_parser->current(), "Expect FeatureLine");
        return;
      }
    }
  } while (!m_parser->check(token_type::eof));
  finish_chunk();
}

void cucumber::compile_feature()
{
  if (tags_valid())
  {
    feature f(this);
    f.compile();
  }
  else
  {
    m_parser->advance();
    m_parser->advance_to(token_type::feature, token_type::tag, token_type::eof);
  }
}

}  // namespace cwt::details::compiler
