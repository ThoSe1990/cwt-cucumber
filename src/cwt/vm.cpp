// TODO remove iostream
#include <iostream>
#include <format>

#include "vm.hpp"
#include "chunk.hpp"
#include "util.hpp"
#include "compiler/cucumber.hpp"

#ifdef PRINT_STACK
#include "debug.hpp"
#endif

namespace cwt::details
{

return_code vm::interpret(std::string_view source)
{
  compiler::cucumber c(source);

  c.compile();

  if (c.no_error())
  {
    m_stack.push_back(c.make_function());
    call(m_stack.back().as<function>());
    run();
    // TODO that can fail in run()
    return return_code::passed;
  }
  else
  {
    return return_code::compile_error;
  }
}
std::vector<step>& vm::steps()
{
  static std::vector<step> instance;
  return instance;
}
void vm::push_step(const step& s) { steps().push_back(s); }
std::vector<hook>& vm::before()
{
  static std::vector<hook> instance;
  return instance;
}
void vm::push_hook_before(const hook& h) { before().push_back(h); }
std::vector<hook>& vm::after()
{
  static std::vector<hook> instance;
  return instance;
}
void vm::push_hook_after(const hook& h) { after().push_back(h); }
std::vector<hook>& vm::before_step()
{
  static std::vector<hook> instance;
  return instance;
}
void vm::push_hook_before_step(const hook& h) { before_step().push_back(h); }
std::vector<hook>& vm::after_step()
{
  static std::vector<hook> instance;
  return instance;
}
void vm::push_hook_after_step(const hook& h) { after_step().push_back(h); }

void vm::runtime_error(std::string_view msg) 
{
  print(color::red, msg);
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
      case op_code::set_var:
      {
        uint32_t next = frame->it.next();
        std::string var =
            frame->chunk_ptr->constant(next).copy_as<std::string>();
        if (m_globals.contains(var))
        {
          m_globals[var] = m_stack.back();
        }
        else
        {
          runtime_error(std::format("Undefined variable '{}'.", var));
        }
        m_stack.pop_back();
      }
      break;
      case op_code::hook:
      {
        hook_type type = to_hook_type(frame->it.next());
        std::cout << "op_code::hook";
        if (type == hook_type::before)
        {
          for (auto& h : before())
          {
            h.call();
          }
        }
        if (type == hook_type::after)
        {
          for (auto& h : after())
          {
            h.call();
          }
        }

        if (type == hook_type::before_step)
        {
          for (auto& h : before_step())
          {
            h.call();
          }
        }
        if (type == hook_type::after_step)
        {
          for (auto& h : after_step())
          {
            h.call();
          }
        }
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
        println(color::red, std::format("steps count: {}", steps().size()));
        for (const auto& s : steps())
        {
          value_array values;
          s(values);
        }
        std::cout << "op_code::call_step: " << name << std::endl;
      }
      break;
      case op_code::step_result:
      {
        std::cout << "op_code::step_result - pop follows... " << std::endl;
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
        println(color::green, "op_code::scenario_result");
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
        print(to_color(frame->it.next()), m_stack.back().as<std::string>());
        m_stack.pop_back();
      }
      break;
      case op_code::println:
      {
        println(to_color(frame->it.next()), m_stack.back().as<std::string>());
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