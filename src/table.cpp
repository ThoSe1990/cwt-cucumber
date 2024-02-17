#include "table.hpp"

namespace cuke
{

void table::append_row(value_array values)
{
  if (m_data.empty())
  {
    m_row_length = values.size();
  }

  if (values.size() == m_row_length)
  {
    for (const value& v : values)
    {
      m_data.push_back(v);
    }
  }
  else
  {
    throw std::runtime_error(
        "Can not append values, values size and row length are not equal");
  }
}

}  // namespace cuke
