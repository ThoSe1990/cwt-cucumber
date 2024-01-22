
#include "cuke_compiler.hpp"
#include "feature.hpp"

namespace cwt::details
{
cuke_compiler::cuke_compiler(std::string_view source) : compiler(source, "") {}
cuke_compiler::cuke_compiler(std::string_view source, std::string_view filename)
    : compiler(source, filename)
{
}
void cuke_compiler::compile()
{
  m_parser->advance();
  m_parser->skip_linebreaks();
  if (m_parser->match(token_type::feature))
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
