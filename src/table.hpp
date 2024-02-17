#pragma once

#include "value.hpp"

namespace cuke
{
class table
{
 public:
  table() = default;
  
  void append_row(value_array values);

// private:
  value_array m_data;
  std::size_t m_row_length{0};
};

}  // namespace cuke

