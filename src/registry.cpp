
#include "registry.hpp"

namespace cuke
{

[[nodiscard]] internal::registry& registry()
{
  static internal::registry instance;
  return instance;
}

}  // namespace cuke
