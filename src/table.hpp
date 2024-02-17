#pragma once

#include "value.hpp"

namespace cuke
{
class table
{
 public:
  table() = default;
  table(value_array data);

  void append_row(value_array values);

// private:
  value_array m_data;
  std::size_t m_row_length;
};

}  // namespace cuke

