#include <format>

#include "compiler.hpp"
#include "token.hpp"

// TODO Remove define
#define PRINT_STACK 1
#include "debug.hpp"

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
  start_function("script");

  m_parser.advance();
  feature();

  return std::make_unique<chunk>(end_function());
}
bool compiler::error() const noexcept { return m_parser.error(); }
bool compiler::no_error() const noexcept { return !error(); }

void compiler::start_function(const std::string& name)
{
  m_chunks.push(chunk(name));
}

chunk compiler::end_function()
{
  m_chunks.top().push_byte(op_code::func_return, m_parser.previous().line);
#ifdef PRINT_STACK
  if (no_error())
  {
    disassemble_chunk(m_chunks.top(), m_chunks.top().name());
    std::cout << "\n";
  }
#endif
  chunk top = std::move(m_chunks.top());
  m_chunks.pop();
  return std::move(top);
}

void compiler::emit_byte(uint32_t byte)
{
  m_chunks.top().push_byte(byte, m_parser.previous().line);
}
void compiler::emit_byte(op_code code)
{
  m_chunks.top().push_byte(code, m_parser.previous().line);
}
void compiler::emit_bytes(op_code code, uint32_t byte)
{
  m_chunks.top().push_byte(code, m_parser.previous().line);
  m_chunks.top().push_byte(byte, m_parser.previous().line);
}
uint32_t compiler::emit_jump()
{
  emit_byte(op_code::jump_if_failed);
  emit_byte(std::numeric_limits<uint32_t>::max());
  return m_chunks.top().size() - 1;
}
void compiler::patch_jump(uint32_t offset)
{
  uint32_t jump = m_chunks.top().size() - 1 - offset;
  if (jump > std::numeric_limits<uint32_t>::max())
  {
    m_parser.error_at(m_parser.previous(), "Too much code to jump over.");
  }

  m_chunks.top().at(offset) = m_chunks.top().size();
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
  // TODO colors 
  emit_bytes(op_code::print, to_uint(color::standard));
  emit_constant(location);
  emit_bytes(op_code::println, to_uint(color::black));
  return m_chunks.top().last_constant() - 1;
}

void compiler::step()
{
  if (m_parser.match(token_type::step))
  {
    const std::size_t step_idx =
        create_name(std::format("{}:{}", m_filename, m_parser.current().line));
  
    emit_byte(op_code::init_scenario);
    uint32_t target_idx = emit_jump();

    emit_hook(hook_type::before_step);
    emit_bytes(op_code::call_step, step_idx);
    emit_hook(hook_type::after_step);

    patch_jump(target_idx);
    emit_bytes(op_code::step_result, step_idx);
    
  }
  else
  {
    m_parser.error_at(m_parser.current(), "Expect StepLine");
  }
}

void compiler::scenario()
{
  if (m_parser.match(token_type::scenario))
  {
    const std::string location =
        std::format("{}:{}", m_filename, m_parser.current().line);
    start_function(location);
    [[maybe_unused]] std::size_t idx = create_name(location);
    // advances to end of line to consume name
    // m_parser.advance_to(token_type::linebreak, token_type::eof);
    m_parser.advance();
    step();

    chunk scenario_chunk = end_function();
    emit_hook(hook_type::reset_context);
    emit_hook(hook_type::before);
    emit_bytes(op_code::constant, m_chunks.top().make_constant(
                                      std::make_unique<chunk>(scenario_chunk)));
    emit_bytes(op_code::define_var,
               m_chunks.top().make_constant(scenario_chunk.name()));
    emit_bytes(op_code::get_var, m_chunks.top().last_constant());
    emit_bytes(op_code::call, 0);
    emit_hook(hook_type::after);
    emit_byte(op_code::scenario_result);
  }
  else
  {
    m_parser.error_at(m_parser.current(), "Expect ScenarioLine");
  }
}
void compiler::feature()
{
  m_parser.skip_linebreaks();
  if (m_parser.match(token_type::feature))
  {
    const std::string location = std::format("{}:{}", m_filename, m_parser.current().line);
    start_function(location);
    [[maybe_unused]] std::size_t idx = create_name(location);
    m_parser.advance();

    m_parser.advance_to(token_type::scenario, token_type::scenario_outline,
                        token_type::tag, token_type::background,
                        token_type::eof);

    scenario();
    chunk feature_chunk = end_function();

    emit_bytes(op_code::constant, m_chunks.top().make_constant(
                                      std::make_unique<chunk>(feature_chunk)));
    emit_bytes(op_code::define_var,
               m_chunks.top().make_constant(feature_chunk.name()));
    emit_bytes(op_code::get_var, m_chunks.top().last_constant());
    emit_bytes(op_code::call, 0);
  }
  else
  {
    m_parser.error_at(m_parser.current(), "Expect FeatureLine");
  }
}

}  // namespace cwt::details
