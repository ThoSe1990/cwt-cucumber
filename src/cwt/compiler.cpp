#include "compiler.hpp"
#include "chunk.hpp"

namespace cwt::details
{

std::string create_string(std::string_view sv) { return std::string(sv); }

compiler::compiler(std::string_view source) : m_scanner(source) {}

function compiler::compile()
{
  function main = start_function("feature");

  advance();
  feature();

  main.chunk_ptr->push_byte(op_code::func_return, m_parser.previous.line);
  return std::move(main);
}

function compiler::start_function(const std::string_view name)
{
  m_enclosing = m_current;
  function new_function{std::string(name), std::make_unique<chunk>()};
  m_current = new_function.chunk_ptr.get();
  return std::move(new_function);
}
void compiler::end_function()
{
  m_current->push_byte(op_code::func_return, m_parser.previous.line);
  m_current = m_enclosing;
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

void compiler::emit_byte(uint32_t byte)
{
  m_current->push_byte(byte, m_parser.previous.line);
}
void compiler::emit_byte(op_code code)
{
  m_current->push_byte(code, m_parser.previous.line);
}
void compiler::emit_bytes(op_code code, uint32_t byte)
{
  m_current->push_byte(code, m_parser.previous.line);
  m_current->push_byte(byte, m_parser.previous.line);
}

void compiler::feature()
{
  consume(token_type::feature, "Expect FeatureLine");

  const std::string name{"TODO Rename me"};
  function func = start_function(name);

  end_function();
  emit_bytes(op_code::constant, m_current->make_constant(std::move(func)));
  emit_bytes(op_code::define_var, m_current->make_constant(name));
  emit_bytes(op_code::get_var, m_current->last_constant());
  emit_bytes(op_code::call, 0);
}

}  // namespace cwt::details
