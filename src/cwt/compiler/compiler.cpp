#include <format>

#include "compiler.hpp"

#ifdef PRINT_STACK
#include "../debug.hpp"
#endif

namespace cwt::details::compiler
{

compiler::compiler(std::string_view source)
    : m_parser(std::make_shared<parser>(source)),
      m_filename(std::string("")),
      m_chunk("script")
{
}
compiler::compiler(std::string_view source, std::string_view filename)
    : m_parser(std::make_shared<parser>(source)),
      m_filename(filename),
      m_chunk("script")
{
}

bool compiler::error() const noexcept { return m_parser->error(); }
bool compiler::no_error() const noexcept { return !error(); }
chunk& compiler::get_chunk() noexcept { return m_chunk; }
void compiler::finish_chunk() noexcept
{
  m_chunk.push_byte(op_code::func_return, m_parser->previous().line);
#ifdef PRINT_STACK
  if (no_error())
  {
    disassemble_chunk(m_chunk, m_chunk.name());
    std::cout << "\n";
  }
#endif
}

std::string compiler::location() const
{
  return std::format("{}:{}", m_filename, m_parser->current().line);
}
std::pair<std::size_t, std::size_t> compiler::create_name_and_location()
{
  std::size_t location_idx = m_chunk.make_constant(location());
  token begin = m_parser->previous();
  m_parser->advance_to(token_type::linebreak, token_type::eof);
  token end = m_parser->previous();
  std::size_t name_idx = m_chunk.make_constant(create_string(begin, end));
  return std::make_pair(name_idx, location_idx);
}
void compiler::print_name_and_location(std::size_t name_idx,
                                       std::size_t location_idx)
{
  emit_bytes(op_code::constant, name_idx);
  emit_bytes(op_code::print, to_uint(color::standard));
  emit_bytes(op_code::constant, location_idx);
  emit_bytes(op_code::println, to_uint(color::black));
}

void compiler::emit_byte(uint32_t byte)
{
  m_chunk.push_byte(byte, m_parser->previous().line);
}
void compiler::emit_byte(op_code code)
{
  m_chunk.push_byte(code, m_parser->previous().line);
}
void compiler::emit_bytes(op_code code, uint32_t byte)
{
  m_chunk.push_byte(code, m_parser->previous().line);
  m_chunk.push_byte(byte, m_parser->previous().line);
}
uint32_t compiler::emit_jump()
{
  emit_byte(op_code::jump_if_failed);
  emit_byte(std::numeric_limits<uint32_t>::max());
  return m_chunk.size() - 1;
}
void compiler::patch_jump(uint32_t offset)
{
  uint32_t jump = m_chunk.size() - 1 - offset;
  if (jump > std::numeric_limits<uint32_t>::max())
  {
    m_parser->error_at(m_parser->previous(), "Too much code to jump over.");
  }

  m_chunk.at(offset) = m_chunk.size();
}

void compiler::emit_hook(hook_type type)
{
  emit_bytes(op_code::hook, to_uint(type));
  if (type == hook_type::before || type == hook_type::after)
  {
    emit_byte(0);  // = args => overload! not all hooks have args
  }
}

void compiler::emit_table_value()
{
  bool negative = m_parser->match(token_type::minus);
  switch (m_parser->current().type)
  {
    case token_type::long_value:
    {
      long v = std::stol(m_parser->current().value.data());
      if (negative)
      {
        v *= -1;
      }
      emit_constant(v);
    }
    break;
    case token_type::double_value:
    {
      double v = std::stod(m_parser->current().value.data());
      if (negative)
      {
        v *= -1;
      }
      emit_constant(v);
    }
    break;
    case token_type::string_value:
    {
      emit_constant(create_string(m_parser->current().value));
    }
    break;
    default:
    {
      m_parser->error_at(m_parser->current(),
                         "Expect a number or string value in table.");
    }
  }
}

}  // namespace cwt::details::compiler
