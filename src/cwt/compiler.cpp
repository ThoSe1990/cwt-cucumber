#include "compiler.hpp"
#include "chunk.hpp"

namespace cwt::details
{

compiler::compiler(std::string_view source) : m_scanner(source)
{

}

function compiler::compile() 
{ 
  function main{"feature", std::make_shared<chunk>()};
  m_current = main.chunk_data;

  advance();
  feature();

  return main; 
}

void compiler::consume(token_type type, std::string_view msg)
{
  if (m_parser.current.type == type)
  {
    advance();
    return ;
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

  m_enclosing = m_current;

  // function{"line:xxx TODO", std::make_shared<chunk>()};
  
  m_current = std::make_shared<chunk>();

  m_enclosing->push_constant(m_parser.previous.line, function{"line:xxx TODO", m_current});
}

}  // namespace cwt::details
