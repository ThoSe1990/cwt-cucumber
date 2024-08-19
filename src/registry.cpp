
#include "registry.hpp"

namespace cuke
{

internal::registry& registry()
{
  static internal::registry instance;
  return instance;
}

}  // namespace cuke
