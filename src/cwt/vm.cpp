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
  auto it = feature.chunk_ptr->cbegin();
  for (;;)
  {
    switch (it.next_as_instruction())
    {
      case op_code::constant:
      {
        std::cout << "constant !!!" << std::endl;
        m_stack.push(feature.chunk_ptr->constant(it.next()));
        std::cout << "stack type: " << static_cast<int>(m_stack.top().type())
                  << std::endl;
      }
      break;
      default:
      {
        std::cout << "defgault!" << std::endl;
      }
    }
    if (it == feature.chunk_ptr->cend()) return;
  }
}

}  // namespace cwt::details