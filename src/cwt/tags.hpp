#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <string_view>

#include "token.hpp"

namespace cwt::details
{

enum class tag_token
{

};

struct tag
{
  token_type token;
  std::string value;
};

class tag_compiler
{
 public:
  tag_compiler(std::string_view expression) {}
  const tag& operator[](std::size_t idx) const
  {
    if (idx < m_data.size()) [[likely]]
    {
      return m_data.at(idx);
    }
    else [[unlikely]]
    {
      throw std::out_of_range("tag_compiler: Access is out of range.");
    }
  }

  std::size_t size() const noexcept { return m_data.size(); }

 private:
  std::vector<tag> m_data;
};

}  // namespace cwt::details
