#include "feature.hpp"
#include "scenario.hpp"
#include "scenario_outline.hpp"

namespace cwt::details::compiler
{

feature::feature(cucumber* enclosing)
    : m_enclosing(enclosing), compiler(*enclosing)
{
  m_parser->advance();
  auto [name_idx, location_idx] = create_name_and_location();
  print_name_and_location(name_idx, location_idx);
  m_parser->advance();
  m_parser->advance_to(token_type::scenario, token_type::scenario_outline,
                       token_type::tag, token_type::background,
                       token_type::eof);
  
}
feature::~feature()
{
  finish_chunk();
  m_enclosing->emit_constant(std::make_unique<chunk>(get_chunk()));
  m_enclosing->emit_constant(op_code::define_var, get_chunk().name());
  m_enclosing->emit_bytes(op_code::get_var,
                          m_enclosing->get_chunk().last_constant());
  m_enclosing->emit_bytes(op_code::call, 0);
}
void feature::compile()
{
  do
  {
    switch (m_parser->current().type)
    {
      case token_type::tag:
      {
        read_tags();
      }
      break;
      case token_type::scenario:
      {
        do_compile<scenario>();
      }
      break;
      case token_type::scenario_outline:
      {
        do_compile<scenario_outline>();
      }
      break;
      default:
      {
        m_parser->error_at(m_parser->current(), "Expect ScenarioLine");
        return;
      }
    }
    m_parser->skip_linebreaks();
  } while (!m_parser->check(token_type::eof));
}

}  // namespace cwt::details::compiler