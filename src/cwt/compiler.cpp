#include <format>

#include "compiler.hpp"
#include "chunk.hpp"

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
    : m_scanner(source), m_filename(std::string(""))
{
}
compiler::compiler(std::string_view source, std::string_view filename)
    : m_scanner(source), m_filename(filename)
{
}

function compiler::compile()
{
  advance();

  function main = start_function("script");
  std::cout << __LINE__ << ":  current: " << m_current << "   enclosing: " << m_enclosing << std::endl;


  feature();

  end_function();
  std::cout << __LINE__ << ":  current: " << m_current << "   enclosing: " << m_enclosing << std::endl;

  return std::move(main);
}
[[nodiscard]] bool compiler::error() const noexcept { return m_parser.error; }
[[nodiscard]] bool compiler::no_error() const noexcept { return !error(); }

void compiler::error_at(const token& t, std::string_view msg) noexcept
{
  std::cerr << "[line " << t.line << "] Error ";
  if (t.type == token_type::eof)
  {
    std::cerr << "at end";
  }
  else if (t.type == token_type::error)
  {
    // nothing
  }
  else
  {
    std::cerr << " at '" << t.value << '\'';
  }
  std::cerr << ": " << msg << '\n';
  m_parser.error = true;
}

function compiler::start_function(const std::string& name)
{
  m_enclosing = m_current;
  function new_function{std::make_unique<chunk>(name)};
  m_current = new_function.get();
  return std::move(new_function);
}
function compiler::start_function()
{
  return std::move(
      start_function(std::format("{}:{}", m_filename, m_parser.current.line)));
}
void compiler::end_function()
{
  m_current->push_byte(op_code::func_return, m_parser.previous.line);

#ifdef PRINT_STACK
  if (no_error())
  {
    disassemble_chunk(*m_current, m_current->name());
    std::cout << "\n";
  }
#endif
  m_current = m_enclosing;
}

bool compiler::check(token_type type) { return m_parser.current.type == type; }
bool compiler::match(token_type type)
{
  if (check(type))
  {
    advance();
    return true;
  }
  else
  {
    return false;
  }
}
void compiler::consume(token_type type, std::string_view msg)
{
  if (m_parser.current.type == type)
  {
    advance();
  }
  else
  {
    error_at(m_parser.current, msg);
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
    error_at(m_parser.current, m_parser.current.value);
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

void compiler::skip_linebreaks()
{
  while (match(token_type::linebreak))
  {
  }
}

void compiler::name()
{
  token begin = m_parser.previous;
  advance_until(token_type::linebreak, token_type::eof);
  token end = m_parser.current;

  emit_constant(create_string(begin, end));
  // TODO second byte = color
  emit_bytes(op_code::print, 0);

  // TODO print location in black/grey
  emit_constant(std::format("{}:{}", m_filename, m_parser.current.line));
  emit_bytes(op_code::println, 0);
}

void compiler::parse_scenarios()
{
  for (;;)
  {
    if (match(token_type::scenario))
    {
      scenario();
    }
    else if (match(token_type::eof))
    {
      break;
    }
    else 
    {
      error_at(m_parser.current, "Expect Scenario.");
    }
  }
}
void compiler::scenario()
{
  consume(token_type::scenario, "Expect FeatureLine");

  function func = start_function();
  std::cout << __LINE__ << ":  current: " << m_current << "   enclosing: " << m_enclosing << std::endl;

  advance_until(token_type::linebreak, token_type::eof);

  end_function();
  std::cout << __LINE__ << ":  current: " << m_current << "   enclosing: " << m_enclosing << std::endl;

}
void compiler::feature()
{
  skip_linebreaks();
  consume(token_type::feature, "Expect FeatureLine");

  function func = start_function();
  std::cout << __LINE__ << ":  current: " << m_current << "   enclosing: " << m_enclosing << std::endl;
  name();
  advance();

  advance_until(token_type::scenario, token_type::scenario_outline,
                token_type::tag, token_type::background, token_type::eof);

  scenario();

  end_function();
  std::cout << __LINE__ << ":  current: " << m_current << "   enclosing: " << m_enclosing << std::endl;

  const std::string function_name = func->name();
  emit_bytes(op_code::constant, m_current->make_constant(std::move(func)));
  emit_bytes(op_code::define_var, m_current->make_constant(function_name));
  emit_bytes(op_code::get_var, m_current->last_constant());
  emit_bytes(op_code::call, 0);
}

}  // namespace cwt::details
