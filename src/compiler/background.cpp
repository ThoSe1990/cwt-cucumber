#include "background.hpp"
#include "step.hpp"

namespace cwt::details::compiler
{

background::background(feature* enclosing) : compiler(*enclosing)
{
  m_lexer->advance_until_line_starts_with(token_type::step);
}

void background::compile()
{
  do
  {
    if (m_lexer->match(token_type::step))
    {
      step s(this);
      s.compile();
    }
    else
    {
      m_lexer->error_at(m_lexer->current(),
                         "Expect StepLine, Scenario or ScenarioOutline");
      return;
    }
    m_lexer->skip_linebreaks();
  } while (m_lexer->is_none_of(token_type::scenario,
                                token_type::scenario_outline, token_type::tag,
                                token_type::eof));

  finish_chunk();
}

}  // namespace cwt::details::compiler