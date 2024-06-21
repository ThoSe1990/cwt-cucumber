#include "cucumber.hpp"
#include "feature.hpp"

#include "../util.hpp"

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
  m_lexer->advance();
  m_lexer->skip_linebreaks();
}

void cucumber::compile()
{
  m_compile_time =
      execute_and_count_time([this]() { this->internal_compile(); });
}

void cucumber::internal_compile()
{
  while (!m_lexer->check(token_type::eof))
  {
    m_lexer->skip_linebreaks();
    switch (m_lexer->current().type)
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
        m_lexer->error_at(m_lexer->current(), "Expect FeatureLine");
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
  m_lexer->advance_to(token_type::feature, token_type::tag, token_type::eof);
}

double cucumber::compile_time() const noexcept { return m_compile_time; }

}  // namespace cwt::details::compiler
