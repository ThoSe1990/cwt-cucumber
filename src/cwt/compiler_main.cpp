#include "compiler.hpp"

namespace cwt::details
{

compiler::main_compiler::main_compiler(compiler* parent)
    : parent_compiler(parent, "script")
{
  parent->m_parser.advance();
}

void compiler::main_compiler::compile_impl()
{
  parent()->m_parser.skip_linebreaks();
  if (parent()->m_parser.match(token_type::feature))
  {
    feature f(parent());
    f.compile();
  }
  else
  {
    parent()->m_parser.error_at(parent()->m_parser.current(),
                                "Expect FeatureLine");
  }
}

}  // namespace cwt::details
