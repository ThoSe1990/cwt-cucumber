#include "compiler.hpp"

namespace cwt::details
{

compiler::feature::feature(compiler* p) : parent_compiler(p, p->location())
{
  [[maybe_unused]] std::size_t idx = parent()->create_name(p->location());
  parent()->m_parser.advance();

  parent()->m_parser.advance_to(token_type::scenario,
                                token_type::scenario_outline, token_type::tag,
                                token_type::background, token_type::eof);
}

compiler::feature::~feature()
{
  chunk feature_chunk = parent()->pop_chunk();

  parent()->emit_constant(std::make_unique<chunk>(feature_chunk));
  parent()->emit_constant(op_code::define_var, feature_chunk.name());
  parent()->emit_bytes(op_code::get_var,
                       parent()->current_chunk().last_constant());
  parent()->emit_bytes(op_code::call, 0);
}

void compiler::feature::compile_impl()
{
  if (parent()->m_parser.match(token_type::scenario))
  {
    scenario s(parent());
    s.compile();
  }
  else
  {
    parent()->m_parser.error_at(parent()->m_parser.current(),
                                "Expect ScenarioLine");
  }
}

}  // namespace cwt::details
