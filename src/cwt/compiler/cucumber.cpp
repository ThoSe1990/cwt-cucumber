
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

value_array compiler::get_all_tags()
{
  value_array result;
  while (m_parser->check(token_type::tag))
  {
    result.push_back(std::string(m_parser->current().value));
    m_parser->advance();
  }
  return result;
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
        m_tags = get_all_tags();
      }
      break;
      case token_type::feature:
      {
        feature f(this);
        f.compile();
      }
      break;
      default:
      {
        m_parser->error_at(m_parser->current(), "Expect FeatureLine");
        return ;
      }
    }
  } while (!m_parser->check(token_type::eof));
  finish_chunk();
}

}  // namespace cwt::details::compiler
