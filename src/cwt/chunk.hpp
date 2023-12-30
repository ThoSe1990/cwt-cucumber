#pragma once

#include <vector>

#include "value.hpp"

namespace cwt::details
{

class chunk
{
 public:
  std::size_t size() const noexcept;

 private:
 private:
  std::size_t m_size;
  std::vector<uint16_t> m_code;
  std::vector<std::size_t> m_lines;
  value_array m_constants;
};

}  // namespace cwt::details
