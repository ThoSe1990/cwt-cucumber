#include <format>

#include "compiler.hpp"

// TODO Remove define
#define PRINT_STACK 1

#ifdef PRINT_STACK
#include "../debug.hpp"
#endif

namespace cwt::details
{

static constexpr std::string create_string(std::string_view sv)
{
  return std::string(sv);
}
static constexpr std::string create_string(std::string_view begin,
                                           std::string_view end)
{
  return std::string(begin.data(), end.data() - begin.data());
}
static constexpr std::string create_string(const token& begin, const token& end)
{
  return create_string(begin.value, end.value);
}

compiler::compiler(std::string_view source)
    : m_parser(std::make_shared<parser>(source)), m_filename(std::string(""))
{
}
compiler::compiler(std::string_view source, std::string_view filename)
    : m_parser(std::make_shared<parser>(source)), m_filename(filename)
{
}

bool compiler::error() const noexcept { return m_parser->error(); }
bool compiler::no_error() const noexcept { return !error(); }
chunk& compiler::get_chunk() noexcept { return m_chunk; }
chunk compiler::take_chunk() noexcept
{
  m_chunk.push_byte(op_code::func_return, m_parser->previous().line);
#ifdef PRINT_STACK
  if (no_error())
  {
    disassemble_chunk(m_chunk, m_chunk.name());
    std::cout << "\n";
  }
#endif
  return std::move(m_chunk);
}

std::string compiler::location() const
{
  return std::format("{}:{}", m_filename, m_parser->current().line);
}
std::size_t compiler::create_name(const std::string& location)
{
  token begin = m_parser->previous();
  m_parser->advance_to(token_type::linebreak, token_type::eof);
  token end = m_parser->current();

  emit_constant(create_string(begin, end));
  emit_bytes(op_code::print, to_uint(color::standard));
  emit_constant(location);
  emit_bytes(op_code::println, to_uint(color::black));
  return m_chunk.last_constant() - 1;
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

}  // namespace cwt::details
