#include "compiler.hpp"

namespace cwt::details
{

compiler::scenario::scenario(compiler* p) : parent_compiler(p, p->location())
{
  [[maybe_unused]] std::size_t idx = parent()->create_name(p->location());
  parent()->m_parser.advance();
}
compiler::scenario::~scenario()
{
  chunk scenario_chunk = parent()->pop_chunk();
  parent()->emit_hook(hook_type::reset_context);
  parent()->emit_hook(hook_type::before);
  parent()->emit_constant(std::make_unique<chunk>(scenario_chunk));
  parent()->emit_constant(op_code::define_var, scenario_chunk.name());
  parent()->emit_bytes(op_code::get_var,
                       parent()->current_chunk().last_constant());
  parent()->emit_bytes(op_code::call, 0);
  parent()->emit_hook(hook_type::after);
  parent()->emit_byte(op_code::scenario_result);
}

void compiler::scenario::compile_impl()
{
  if (parent()->m_parser.match(token_type::step))
  {
    const std::size_t step_idx = parent()->create_name(parent()->location());

    parent()->emit_byte(op_code::init_scenario);
    uint32_t jump = parent()->emit_jump();

    parent()->emit_hook(hook_type::before_step);
    parent()->emit_bytes(op_code::call_step, step_idx);
    parent()->emit_hook(hook_type::after_step);

    parent()->patch_jump(jump);
    parent()->emit_bytes(op_code::step_result, step_idx);
  }
  else
  {
    parent()->m_parser.error_at(parent()->m_parser.current(),
                                "Expect StepLine");
  }
}

}  // namespace cwt::details
