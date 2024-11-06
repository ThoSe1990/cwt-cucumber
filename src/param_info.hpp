#pragma once
#include <string>

namespace cuke::internal
{
struct param_info
{
  std::size_t offset;
  std::size_t param_count;
  std::string description;
};
}  // namespace cuke::internal
