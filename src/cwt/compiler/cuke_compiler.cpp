
#include "cuke_compiler.hpp"
#include "feature.hpp"

namespace cwt::details
{
cuke_compiler::cuke_compiler(std::string_view source) : compiler(source, "")
{
  m_parser->advance();
  m_parser->skip_linebreaks();
}
cuke_compiler::cuke_compiler(std::string_view source, std::string_view filename)
    : compiler(source, filename)
{
  m_parser->advance();
  m_parser->skip_linebreaks();
}
function cuke_compiler::make_function() noexcept
{
  return std::make_unique<chunk>(take_chunk());
}
void cuke_compiler::compile()
{
  if (m_parser->check(token_type::feature))
  {
    feature f(this);
    f.compile();
  }
  else
  {
    m_parser->error_at(m_parser->current(), "Expect FeatureLine");
  }
}

}  // namespace cwt::details
