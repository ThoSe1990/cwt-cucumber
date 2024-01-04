#include "compiler.hpp"
#include "chunk.hpp"

namespace cwt::details
{

compiler::compiler(std::string_view source) : m_scanner(source) {}

function compiler::compile()
{
  function main = start_function("feature");

  advance();
  feature();

  m_current->push_byte(op_code::func_return, m_parser.previous.line);
  return std::move(main);
}

function compiler::start_function(const std::string_view name)
{
  m_enclosing = m_current;
  function new_function{name.data(), std::make_unique<chunk>()};
  m_current = new_function.chunk_data.get();
  return std::move(new_function);
}
void compiler::end_function(function&& func)
{
  m_current->push_byte(op_code::func_return, m_parser.previous.line);
  m_current = m_enclosing;
  m_current->emplace_constant(m_parser.previous.line, std::move(func));
}

void compiler::consume(token_type type, std::string_view msg)
{
  if (m_parser.current.type == type)
  {
    advance();
    return;
  }
  else
  {
    // TODO error at current;
  }
}

void compiler::advance()
{
  m_parser.previous = m_parser.current;
  for (;;)
  {
    m_parser.current = m_scanner.scan_token();
    if (m_parser.current.type != token_type::error)
    {
      break;
    }
    // TODO error at current;
  }
}

void compiler::feature()
{
  consume(token_type::feature, "Expect FeatureLine");

  function feature_func = start_function("feature");

  end_function(std::move(feature_func));
}

}  // namespace cwt::details
