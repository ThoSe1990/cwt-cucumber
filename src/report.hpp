#pragma once 

#include <string>

namespace cuke::report 
{

[[nodiscard]] std::string as_json(std::size_t indents = 2);
void print_json_to_sink(std::size_t indents = 2);

} // namespace cuke::report
