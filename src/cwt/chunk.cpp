#include <stdexcept>

#include "chunk.hpp"
#include "value.hpp"

namespace cwt::details
{
chunk::chunk(const std::string& name) : m_name(name) {}
const std::string& chunk::name() const noexcept { return m_name; }
std::size_t chunk::size() const noexcept { return m_code.size(); }
std::size_t chunk::constants_count() const noexcept
{
  return m_constants.size();
}
std::size_t chunk::last_constant() const noexcept
{
  return m_constants.size() - 1;
}

const uint32_t& chunk::back() const noexcept { return m_code.back(); }
const value& chunk::constant(std::size_t index) const
{
  if (index < m_constants.size()) [[likely]]
  {
    return m_constants[index];
  }
  else [[unlikely]]
  {
    throw std::out_of_range("Chunk: Constants out of range");
  }
}
const value& chunk::constants_back() const noexcept
{
  return m_constants.back();
}

[[nodiscard]] uint32_t chunk::lines(const std::size_t index) const
{
  if (index < m_lines.size()) [[likely]]
  {
    return m_lines[index];
  }
  else [[unlikely]]
  {
    throw std::out_of_range("Chunk: Lines access out of range");
  }
}

void chunk::push_byte(op_code byte, const std::size_t line)
{
  push_byte(static_cast<uint32_t>(byte), line);
}
void chunk::push_byte(uint32_t byte, const std::size_t line)
{
  m_code.push_back(byte);
  m_lines.push_back(line);
}

chunk::const_iterator chunk::cbegin() const
{
  return chunk::const_iterator(m_code.cbegin());
}
chunk::const_iterator chunk::cend() const
{
  return chunk::const_iterator(m_code.cend());
}

uint32_t chunk::at(std::size_t index) const { return m_code[index]; }
uint32_t chunk::operator[](std::size_t index) const
{
  if (index < m_code.size()) [[likely]]
  {
    return m_code[index];
  }
  else [[unlikely]]
  {
    throw std::out_of_range("Chunk Operator[]: Byte out of range");
  }
}

std::size_t chunk::get_index(chunk::const_iterator iter) const
{
  return std::distance(cbegin(), iter);
}

const uint32_t& chunk::const_iterator::operator*() const { return *m_current; }
uint32_t chunk::const_iterator::next()
{
  uint32_t next = *m_current;
  ++m_current;
  return next;
}
chunk::const_iterator& chunk::const_iterator::operator++()
{
  ++m_current;
  return *this;
}
bool chunk::const_iterator::operator==(const chunk::const_iterator& rhs) const
{
  return m_current == rhs.m_current;
}
bool chunk::const_iterator::operator!=(const chunk::const_iterator& rhs) const
{
  return m_current != rhs.m_current;
}

}  // namespace cwt::details
