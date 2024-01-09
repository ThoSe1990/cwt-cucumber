#include <iostream>

#include "vm.hpp"
#include "compiler.hpp"
#include "chunk.hpp"

// TODO Remove
#define PRINT_STACK 1

#ifdef PRINT_STACK
#include "debug.hpp"
#endif

namespace cwt::details
{

void vm::interpret(std::string_view source)
{
  compiler c(source);
  function feature = c.compile();

  if (c.no_error())
  {
    m_stack.push_back(std::move(feature));
    call(m_stack.back().as<function>());
    run();
  }
}

void vm::call(const function& func)
{
  m_frames.push_back(call_frame{func.get(), func->cbegin()});
}

void vm::run()
{
  call_frame* frame = &m_frames.back();
  for (;;)
  {
#ifdef PRINT_STACK
    for (const auto& v : m_stack)
    {
      std::cout << "[ ";
      print_value(v);
      std::cout << " ]";
    }
    std::cout << '\n';
    disassemble_instruction(*frame->chunk_ptr,
                            frame->chunk_ptr->get_index(frame->it));
#endif

    switch (to_code(frame->it.next()))
    {
      case op_code::constant:
      {
        uint32_t next = frame->it.next();
        m_stack.push_back(std::move(frame->chunk_ptr->constant(next)));
      }
      break;
      case op_code::define_var:
      {
        uint32_t next = frame->it.next();
        std::string name =
            frame->chunk_ptr->constant(next).copy_as<std::string>();
        m_globals[name] = m_stack.back();
        m_stack.pop_back();
      }
      break;
      case op_code::get_var:
      {
        uint32_t next = frame->it.next();
        std::string name =
            frame->chunk_ptr->constant(next).copy_as<std::string>();
        m_stack.push_back(m_globals[name]);
      }
      break;
      case op_code::call:
      {
        uint32_t argc = frame->it.next();  // this is arg count
        call(m_stack.back().as<function>());
        frame = &m_frames.back();
      }
      break;
      case op_code::print_line:
      {
        // std::cout << "here we print some stuff!!!" << std::endl;
      }
      break;
      case op_code::func_return:
      {
        m_frames.pop_back();
        if (m_frames.empty())
        {
          m_stack.pop_back();
          return;
        }
        else
        {
          frame = &m_frames.back();
        }
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