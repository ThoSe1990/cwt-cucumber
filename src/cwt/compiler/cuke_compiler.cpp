
#include "cuke_compiler.hpp"
#include "feature.hpp"

namespace cwt::details
{
cuke_compiler::cuke_compiler(std::string_view source) : compiler(source, "")
{
  init();
}
cuke_compiler::cuke_compiler(std::string_view source, std::string_view filename)
    : compiler(source, filename)
{
  init();
}
function cuke_compiler::make_function() noexcept
{
  return std::make_unique<chunk>(get_chunk());
}
void cuke_compiler::init()
{
  m_parser->advance();
  m_parser->skip_linebreaks();
}
void cuke_compiler::compile()
{
  switch (m_parser->current().type)
  {
    case token_type::feature:
    {
      feature f(this);
      f.compile();
    }
    break;
    default:
    {
      m_parser->error_at(m_parser->current(), "Expect FeatureLine");
    }
  }

  finish_chunk();
}

}  // namespace cwt::details
