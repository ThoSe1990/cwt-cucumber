#include <iostream>
#include "vm.hpp"
#include "compiler.hpp"
#include "chunk.hpp"
namespace cwt::details
{

void vm::interpret(std::string_view source)
{
  compiler c(source);
  function feature = c.compile();

  m_stack.push(std::move(feature));
  call(m_stack.top().as<function>());
  run();
}

void vm::call(const function& func)
{
  m_frames.emplace_back(call_frame{func.chunk_ptr.get()});
}

void vm::run()
{
  call_frame frame = m_frames.back();
  auto it = frame.chunk_ptr->cbegin();
  for (;;)
  {
    switch (it.next_as_instruction())
    {
      case op_code::constant:
      {
        uint32_t next = it.next();
        std::cout << "next is " << next;
        std::cout << " constant " << static_cast<int>(frame.chunk_ptr->constant(next).type());
        // m_stack.push(frame.chunk_ptr->constant(it.next()));
        m_stack.push(std::move(frame.chunk_ptr->constant(next)));
        std::cout << " stack type: " << static_cast<int>(m_stack.top().type())
                  << std::endl;
      }
      break;
      case op_code::func_return:
      {
        std::cout << "func_return" << std::endl;
        m_frames.pop_back();
        if (m_frames.empty())
        {
          m_stack.pop();
          return;
        }
        else
        {
          frame = m_frames.back();
        }
      }
      break;
      case op_code::define_var:
      {
        std::cout << "define_var" << std::endl;
        m_stack.push(frame.chunk_ptr->constant(it.next()).as<std::string>());
      }
      break;
      case op_code::call:
      {
        std::cout << "call stack top: " << static_cast<int>(m_stack.top().type()) << " size: " << m_stack.size() << std::endl;
        it.next(); // this is arg count
      }
      break;
      default:
      {
        std::cout << "default!" << std::endl;
      }
    }
  }
}

}  // namespace cwt::details