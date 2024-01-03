#include <iostream>
#include "vm.hpp"
#include "compiler.hpp"
#include "chunk.hpp"
namespace cwt::details
{

void vm::run(std::string_view script)
{
  compiler c(script);
  function feature = c.compile();

  for (auto it = feature.chunk_data->cbegin(); it != feature.chunk_data->cend();
       ++it)
  {
    std::cout << static_cast<unsigned int>(*it) << std::endl;
    std::cout << feature.name << std::endl;
    std::cout << feature.chunk_data->constants_count() << std::endl;
    std::cout << static_cast<int>(feature.chunk_data->constant(0).type()) << std::endl;
  }
}

}  // namespace cwt::details