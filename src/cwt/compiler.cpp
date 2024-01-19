#include <format>
// TODO Remove define
#define PRINT_STACK 1
#include "debug.hpp"

#include "compiler.hpp"

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
    : m_parser(source), m_filename(std::string(""))
{
}
compiler::compiler(std::string_view source, std::string_view filename)
    : m_parser(source), m_filename(filename)
{
}

function compiler::compile()
{
  main_compiler m(this);
  m.compile();
  return std::make_unique<chunk>(pop_chunk());
}
bool compiler::error() const noexcept { return m_parser.error(); }
bool compiler::no_error() const noexcept { return !error(); }
chunk& compiler::current_chunk() { return m_chunks.top(); }

chunk compiler::pop_chunk()
{
  current_chunk().push_byte(op_code::func_return,
                                      m_parser.previous().line);
#ifdef PRINT_STACK
  if (no_error())
  {
    disassemble_chunk(current_chunk(),
                      current_chunk().name());
    std::cout << "\n";
  }
#endif
  chunk top = std::move(current_chunk());
  m_chunks.pop();
  return std::move(top);
}
void compiler::push_chunk(const std::string& name)
{
  m_chunks.push(chunk(name));
}

std::string compiler::location() const
{
  return std::format("{}:{}", m_filename, m_parser.current().line);
}
void compiler::emit_byte(uint32_t byte)
{
  current_chunk().push_byte(byte, m_parser.previous().line);
}
void compiler::emit_byte(op_code code)
{
  current_chunk().push_byte(code, m_parser.previous().line);
}
void compiler::emit_bytes(op_code code, uint32_t byte)
{
  current_chunk().push_byte(code, m_parser.previous().line);
  current_chunk().push_byte(byte, m_parser.previous().line);
}
uint32_t compiler::emit_jump()
{
  emit_byte(op_code::jump_if_failed);
  emit_byte(std::numeric_limits<uint32_t>::max());
  return current_chunk().size() - 1;
}
void compiler::patch_jump(uint32_t offset)
{
  uint32_t jump = current_chunk().size() - 1 - offset;
  if (jump > std::numeric_limits<uint32_t>::max())
  {
    m_parser.error_at(m_parser.previous(), "Too much code to jump over.");
  }

  current_chunk().at(offset) = current_chunk().size();
}

void compiler::emit_hook(hook_type type)
{
  emit_bytes(op_code::hook, to_uint(type));
  if (type == hook_type::before || type == hook_type::after)
  {
    emit_byte(0);  // = args => overload! not all hooks have args
  }
}

std::size_t compiler::create_name(const std::string& location)
{
  token begin = m_parser.previous();
  m_parser.advance_to(token_type::linebreak, token_type::eof);
  token end = m_parser.current();

  emit_constant(create_string(begin, end));
  emit_bytes(op_code::print, to_uint(color::standard));
  emit_constant(location);
  emit_bytes(op_code::println, to_uint(color::black));
  return current_chunk().last_constant() - 1;
}

}  // namespace cwt::details
