#pragma once

#include <memory>

#include "scanner.hpp"
#include "token.hpp"
#include "value.hpp"

namespace cwt::details
{

struct parser
{
  token current;
  token previous;
  bool error{false};
};


class compiler
{
 public:
  compiler(std::string_view source);
  [[nodiscard]] function compile();

 private:
  [[nodiscard]] function start_function(const std::string_view name);
  void end_function();
  void advance();
  void consume(token_type type, std::string_view msg);
  
  void feature(); 

 private:
  scanner m_scanner;
  parser m_parser;
  chunk* m_current;
  chunk* m_enclosing;
};

}  // namespace cwt::details
