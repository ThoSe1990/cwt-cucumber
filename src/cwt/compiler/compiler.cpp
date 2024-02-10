#include <format>

#include "compiler.hpp"

#ifdef PRINT_STACK
#include "../debug.hpp"
#endif

namespace cwt::details::compiler
{

compiler::compiler(std::string_view source)
    : m_parser(std::make_shared<parser>(source)),
      m_options(std::make_shared<options>()),
      m_lines({}),
      m_filename("line"),
      m_chunk("script")
{
}
compiler::compiler(const file& feature_file)
    : m_parser(std::make_shared<parser>(feature_file.content)),
      m_options(std::make_shared<options>()),
      m_lines(feature_file.lines),
      m_filename(feature_file.path),
      m_chunk("script")
{
}

const options& compiler::get_options() const { return *m_options.get(); }
void compiler::set_options(const options& opts)
{
  m_options = std::make_shared<options>(opts);
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

value_array compiler::take_latest_tags()
{
  value_array result = *m_latest_tags.get();
  m_latest_tags->clear();
  return result;
}
bool compiler::tags_valid(const value_array& tags)
{
  return m_options->tags.evaluate(tags.size(), tags.rbegin());
}
void compiler::read_tags()
{
  m_latest_tags->clear();
  while (m_parser->check(token_type::tag))
  {
    m_latest_tags->push_back(std::string(m_parser->current().value));
    m_parser->advance();
  }
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

void compiler::emit_table_value()
{
  bool negative = m_parser->match(token_type::minus);
  emit_constant(token_to_value(m_parser->current(), negative));
}

void compiler::emit_tags(const value_array& tags)
{
  // TODO: if we introduce a find function for constants in chunk
  // we can avoid pushing redundand tags, which always happens in
  // before and after because all tags are pushed twice (at least...)
  for (const auto t : tags)
  {
    emit_constant(t.as<std::string>());
  }
}

}  // namespace cwt::details::compiler
