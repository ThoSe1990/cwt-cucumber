#pragma once

#include "chunk.hpp"

namespace cuke::internal
{
class function
{
 public:
  const chunk& get_chunk() const { return m_chunk; }

 private:
  std::size_t m_arity;
  chunk m_chunk;
  std::string m_name;
};
}  // namespace cuke::internal
