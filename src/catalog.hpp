
#include <string>

namespace cuke::catalog
{
[[nodiscard]] std::string as_readable_text();
[[nodiscard]] std::string as_json(std::size_t indents = 2);

}  // namespace cuke::catalog
