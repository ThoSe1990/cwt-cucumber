#include "table.hpp"

namespace cuke
{

table::table(value_array data, std::size_t row_length) : m_data(std::move(data)), m_row_length(row_length) {}

[[nodiscard]] std::size_t table::rows_count() const noexcept
{
  return m_data.size() / m_row_length;
}
[[nodiscard]] std::size_t table::row_length() const noexcept
{
  return m_row_length;
}
[[nodiscard]] std::size_t table::cells_count() const noexcept
{
  return m_data.size();
}

}  // namespace cuke
