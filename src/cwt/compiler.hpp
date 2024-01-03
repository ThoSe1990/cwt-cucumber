#include "token.hpp"
#include "object.hpp"

namespace cwt::details
{

struct parser
{
  token current;
  token previous;
  bool error;
};


class compiler
{
 public:
  [[nodiscard]] function compile(std::string_view source);

 private:
 private:
  // parser m_parser;
  // function m_function;
};

}  // namespace cwt::details
