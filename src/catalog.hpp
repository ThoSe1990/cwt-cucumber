
#include <string>

namespace cuke::catalog
{
[[nodiscard]] std::string as_readable_text();
[[nodiscard]] std::string as_json(std::size_t indents = 2);
void print_readable_text_to_sink();
void print_json_to_sink(std::size_t indents = 2);

}  // namespace cuke::catalog
