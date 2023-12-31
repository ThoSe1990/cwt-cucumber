#pragma once

#include "chunk.hpp"

namespace cwt::details
{

std::size_t chunk::size() const noexcept { return m_code.size(); }
std::size_t chunk::constants_count() const noexcept
{
  return m_constants.size();
}

void chunk::push_byte(op_code byte, std::size_t line)
{
  push_byte(static_cast<uint32_t>(byte), line);
}
void chunk::push_byte(uint32_t byte, std::size_t line)
{
  m_code.push_back(byte);
  m_lines.push_back(line);
}
void chunk::add_constant(value&& v)
{
  m_constants.push_back(std::forward<value>(v));
}

chunk::iterator chunk::begin() const { return chunk::iterator(m_code.cbegin()); }
chunk::iterator chunk::end() const { return chunk::iterator(m_code.cend()); }

const uint32_t& chunk::iterator::operator*() const { return *m_current; }
chunk::iterator& chunk::iterator::operator++()
{
  ++m_current;
  return *this;
}
bool chunk::iterator::operator==(const chunk::iterator& rhs) const
{
  return m_current == rhs.m_current;
}
bool chunk::iterator::operator!=(const chunk::iterator& rhs) const
{
  return m_current != rhs.m_current;
}

}  // namespace cwt::details
