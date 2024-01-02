#include <string>

#include "chunk.hpp"

namespace cwt::details
{

struct function
{
  const std::size_t arity;
  chunk func_chunk;
  const std::string name;
};

using cuke_step = void (*)(const value_array&);

struct native
{
  cuke_step func;
};

}  // namespace cwt::details
