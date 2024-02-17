#include "table.hpp"

namespace cuke
{

table::table(value_array data) : m_data(data) {}

void table::append_row(value_array values)
{
  if (values.size() != m_row_length) [[unlikely]]
  {
    throw std::runtime_error(
        "Can not append values, values size and row length are not equal");
  }
  else
  {
    for (const value& v : values)
    {
      m_data.push_back(v);
    }
  }
}

}  // namespace cuke
