#pragma once

#include "../value.hpp"

namespace cuke
{
namespace internal 
{
class registry
{
 public:
  void push_step(const internal::step& s) noexcept
  {
    get_steps().push_back(s);
  }
  void clear() noexcept
  {
    get_steps().clear();
  }
  [[nodiscard]] const std::vector<internal::step>& steps() noexcept
  {
    return get_steps(); 
  }
 private:
  [[nodiscard]] std::vector<internal::step>& get_steps() noexcept
  {
    static std::vector<internal::step> instance; 
    return instance; 
  }
};

} // namespace internal 

[[nodiscard]] internal::registry& registry();

} // namespace cuke
