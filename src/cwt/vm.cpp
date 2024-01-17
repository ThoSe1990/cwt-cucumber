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

void vm::push_step(const step& s) { m_steps.push_back(s); }
void vm::push_before(const hook& h) { m_before.push_back(h); }
void vm::push_after(const hook& h) { m_after.push_back(h); }
void vm::push_before_step(const hook& h) { m_before_step.push_back(h); }
void vm::push_after_step(const hook& h) { m_after_step.push_back(h); }

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

    // TODO: switch (to_code(frame->it.next()))
    uint32_t current = frame->it.next();
    switch (to_code(current))
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
      case op_code::hook:
      {
        hook_type type = to_hook_type(frame->it.next());
        std::cout << "op_code::hook";
        if (type == hook_type::before || type == hook_type::after)
        {
          uint32_t tags = frame->it.next();
          std::cout << ": tags count = " << tags;
        }
        std::cout << std::endl;
      }
      break;
      case op_code::call_step:
      {
        uint32_t next = frame->it.next();
        std::string name =
            frame->chunk_ptr->constant(next).copy_as<std::string>();
        // TODO -> of course no loop here, just demonstartaion!
        for (const auto& s : m_steps)
        {
          value_array values;
          s(values);
        }
        std::cout << "op_code::call_step: " << name << std::endl;
      }
      break;
      case op_code::step_result:
      {
        uint32_t next = frame->it.next();
        std::string name =
            frame->chunk_ptr->constant(next).copy_as<std::string>();
        std::cout << "op_code::step_result: " << name << std::endl;
      }
      break;
      case op_code::call:
      {
        uint32_t argc = frame->it.next();  // this is arg count
        call(m_stack.back().as<function>());
        frame = &m_frames.back();
      }
      break;
      case op_code::scenario_result:
      {
        std::cout << "op_code::scenario_result" << std::endl;
      }
      break;
      case op_code::jump_if_failed:
      {
        uint32_t target = frame->it.next();
        std::cout << "op_code::jump_if_failed: " << target << std::endl;
      }
      break;
      case op_code::print:
      {
        uint32_t color = frame->it.next();
        std::cout << "** Color: " << color << "** " << m_stack.back().as<std::string>() << std::endl;
        m_stack.pop_back();
      }
      break;
      case op_code::println:
      {
        uint32_t color = frame->it.next();
        std::cout << "** Color: " << color << "** " << m_stack.back().as<std::string>()
                  << '\n';
        m_stack.pop_back();
      }
      break;
      case op_code::init_scenario:
      {
        std::cout << "op_code::init_scenario" << std::endl;
      }
      break;
      case op_code::func_return:
      {
        m_stack.pop_back();
        m_frames.pop_back();
        if (m_frames.empty())
        {
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
        std::cout << "default! Missing op_code: " << current << std::endl;
      }
    }
  }
}

}  // namespace cwt::details