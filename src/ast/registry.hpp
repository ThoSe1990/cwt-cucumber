#pragma once

#include "../value.hpp"

namespace cuke
{

class registry
{
 public:
  static void push_step(const internal::step& s) noexcept
  {
    get_steps().push_back(s);
  }
  static void clear() noexcept
  {
    get_steps().clear();
  }
  [[nodiscard]] static const std::vector<internal::step>& steps() noexcept
  {
    return get_steps(); 
  }
 private:
  [[nodiscard]] static std::vector<internal::step>& get_steps() noexcept
  {
    static std::vector<internal::step> instance; 
    return instance; 
  }
};

}  // namespace cuke
