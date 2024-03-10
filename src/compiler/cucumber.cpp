
#include "cucumber.hpp"
#include "feature.hpp"

namespace cwt::details::compiler
{
cucumber::cucumber(std::string_view source) : compiler(source) { init(); }
cucumber::cucumber(const file& feature_file) : compiler(feature_file)
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

void cucumber::compile()
{
  while (!m_parser->check(token_type::eof))
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
  }
  finish_chunk();
}

void cucumber::compile_feature()
{
  const cuke::value_array tags = take_latest_tags();
  feature f(this, tags);
  f.compile();
  m_parser->advance_to(token_type::feature, token_type::tag, token_type::eof);
}

}  // namespace cwt::details::compiler
