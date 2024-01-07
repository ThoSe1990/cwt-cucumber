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

  m_stack.push(std::move(feature));
  call(m_stack.top().as<function>());
  run();
}

void vm::call(const function& func)
{
  m_frames.push_back(
      call_frame{func.chunk_ptr.get(), func.chunk_ptr->cbegin()});
}

void vm::run()
{
  call_frame* frame = &m_frames.back();
  
#ifdef PRINT_STACK
  disassemble_chunk(*frame->chunk_ptr, "main");
  std::cout << "\n---\n\n";
#endif
  for (;;)
  {
// #ifdef PRINT_STACK
  // for (const auto& v : m_stack)
  // {
  //   std::cout << "[ ";
  //   print_value(v);
  //   std::cout << " ]";
  // }
  // std::cout << '\n';
  // disassemble_instruction(*frame->chunk_ptr, frame->it.current());
// #endif


// call / change disassemble_instruction with the actual instruction and not with index ... 
// wont help to same behavior bc line is still missing then but i dont have an index here ... 
// then change to vector from std::stack for random access 
  std::cout << "current instruction: " << frame->it.current() << std::endl; 
    switch (frame->it.next_as_instruction())
    {
      case op_code::constant:
      {
        uint32_t next = frame->it.next();
        std::cout << "op_code::constant: "
                  << static_cast<int>(frame->chunk_ptr->constant(next).type())
                  << std::endl;
        m_stack.push(std::move(frame->chunk_ptr->constant(next)));
      }
      break;
      case op_code::define_var:
      {
        uint32_t next = frame->it.next();
        std::string name =
            frame->chunk_ptr->constant(next).copy_as<std::string>();
        m_globals[name] = m_stack.top();
        m_stack.pop();

        std::cout << "op_code::define_var globals: "
                  << static_cast<int>(m_globals[name].type()) << std::endl;
      }
      break;
      case op_code::get_var:
      {
        uint32_t next = frame->it.next();
        std::string name =
            frame->chunk_ptr->constant(next).copy_as<std::string>();
        std::cout << "op_code::get_var globals: "
                  << static_cast<int>(m_globals[name].type()) << std::endl;
        m_stack.push(m_globals[name]);
      }
      break;
      case op_code::call:
      {
        std::cout << "op_code::call stack top: "
                  << static_cast<int>(m_stack.top().type()) << std::endl;

        uint32_t argc = frame->it.next();  // this is arg count
        call(m_stack.top().as<function>());
        frame = &m_frames.back();
      }
      break;
      case op_code::print_line:
      {
        std::cout << "here we print some stuff!!!" << std::endl;
      }
      break;
      case op_code::func_return:
      {
        std::cout << "func_return" << std::endl;
        m_frames.pop_back();
        if (m_frames.empty())
        {
          m_stack.pop();
          std::cout << "we're done, stack size: " << m_stack.size() << std::endl;
          std::cout << "with main feature function left: " << std::endl;
          std::cout << "   stack top type: " <<  static_cast<int>(m_stack.top().type()) << std::endl;
          std::cout << "   function.name: " << m_stack.top().as<function>().name << std::endl;
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