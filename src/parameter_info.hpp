#include <string>

namespace cuke::internal
{

struct parameter_info
{
  std::string key;
  std::string info_text;
  std::size_t offset;
};

}  // namespace cuke::internal
