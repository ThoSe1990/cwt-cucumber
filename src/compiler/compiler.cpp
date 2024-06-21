#include <format>

#include "compiler.hpp"

#ifdef PRINT_STACK
#include "../debug.hpp"
#endif

namespace cwt::details::compiler
{

compiler::compiler(std::string_view source)
    : m_lexer(std::make_shared<lexer>(source)),
      m_options(std::make_shared<options>()),
      m_lines({}),
      m_filename("line"),
      m_chunk("script")
{
}
compiler::compiler(const file& feature_file)
    : m_lexer(std::make_shared<lexer>(feature_file)),
      m_options(std::make_shared<options>()),
      m_lines(feature_file.lines_to_compile),
      m_filename(feature_file.path),
      m_chunk("script")
{
}

const options& compiler::get_options() const { return *m_options.get(); }
void compiler::set_options(const options& opts)
{
  m_options = std::make_shared<options>(opts);
}
bool compiler::error() const noexcept { return m_lexer->error(); }
bool compiler::no_error() const noexcept { return !error(); }
chunk& compiler::get_chunk() noexcept { return m_chunk; }
void compiler::finish_chunk() noexcept
{
  m_chunk.push_byte(op_code::func_return, m_lexer->previous().line);
#ifdef PRINT_STACK
  if (no_error())
  {
    disassemble_chunk(m_chunk, m_chunk.name());
    std::cout << "\n";
  }
#endif
}

cuke::value_array compiler::take_latest_tags()
{
  cuke::value_array result = *m_latest_tags.get();
  m_latest_tags->clear();
  return result;
}
bool compiler::tags_valid(const cuke::value_array& tags)
{
  return m_options->tags.evaluate(tags.size(), tags.rbegin());
}
void compiler::read_tags()
{
  m_latest_tags->clear();
  while (m_lexer->check(token_type::tag))
  {
    m_latest_tags->push_back(std::string(m_lexer->current().value));
    m_lexer->advance();
  }
}

std::string compiler::location() const
{
  return std::format("{}:{}", m_filename, m_lexer->current().line);
}
std::pair<std::size_t, std::size_t> compiler::create_name_and_location()
{
  std::size_t location_idx = m_chunk.make_constant(location());
  token begin = m_lexer->previous();
  m_lexer->advance_to(token_type::linebreak, token_type::eof);
  token end = m_lexer->previous();
  std::size_t name_idx = m_chunk.make_constant(create_string(begin, end));
  return std::make_pair(name_idx, location_idx);
}
void compiler::print_name_and_location(std::size_t name_idx,
                                       std::size_t location_idx)
{
  if (!m_options->quiet)
  {
    emit_byte(op_code::print_linebreak);
    emit_bytes(op_code::constant, name_idx);
    emit_bytes(op_code::print, to_uint(color::standard));
    emit_byte(op_code::print_indent);
    emit_bytes(op_code::constant, location_idx);
    emit_bytes(op_code::println, to_uint(color::black));
  }
}

void compiler::emit_byte(uint32_t byte)
{
  m_chunk.push_byte(byte, m_lexer->previous().line);
}
void compiler::emit_byte(op_code code)
{
  m_chunk.push_byte(code, m_lexer->previous().line);
}
void compiler::emit_bytes(op_code code, uint32_t byte)
{
  m_chunk.push_byte(code, m_lexer->previous().line);
  m_chunk.push_byte(byte, m_lexer->previous().line);
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
    m_lexer->error_at(m_lexer->previous(), "Too much code to jump over.");
  }

  m_chunk.at(offset) = m_chunk.size();
}


void compiler::emit_tags(const cuke::value_array& tags)
{
  // TODO: if we introduce a find function for constants in chunk
  // we can avoid pushing redundand tags, which always happens in
  // before and after because all tags are pushed twice (at least...)
  for (const auto t : tags)
  {
    emit_constant(t.as<std::string>());
  }
}

bool compiler::lines_match() const noexcept
{
  if (m_lines.empty())
  {
    return true;
  }
  else
  {
    std::size_t current_line = m_lexer->current().line;
    return std::any_of(m_lines.begin(), m_lines.end(),
                       [&current_line](std::size_t line)
                       { return current_line == line; });
  }
}

}  // namespace cwt::details::compiler
