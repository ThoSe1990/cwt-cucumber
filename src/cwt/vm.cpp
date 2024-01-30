// TODO remove iostream
#include <format>
#include <algorithm>

#include "vm.hpp"
#include "util.hpp"
#include "chunk.hpp"
#include "step_finder.hpp"
#include "compiler/cucumber.hpp"

// TODO Remove
#define PRINT_STACK 1
#ifdef PRINT_STACK
#include "debug.hpp"
#endif

namespace cwt::details
{

return_code vm::interpret(std::string_view source)
{
  // FYI a call frame stores the current frame as raw pointer,
  //  pointing to the chunk_ptr / function on the stack
  //  if the vector reallocates when capacity is exceeded
  //  we loose the rawpointer to the chunk in the current call frame
  //  for now the stack is under control and will most likely not m_exceed
  //  max_stack_size only if a step has more than ~250 variables which is rather
  //  unlikely
  m_stack.reserve(m_max_stack_size);
  compiler::cucumber c(source);

  c.compile();

  if (c.no_error())
  {
    push_value(c.make_function());
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
void vm::push_value(const value& v) { m_stack.push_back(v); }
void vm::pop() { m_stack.pop_back(); }
void vm::pop(std::size_t count)
{
  for (int i = 0; i < count; ++i)
  {
    m_stack.pop_back();
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
  std::size_t idx = m_frames.back().chunk_ptr->get_index(m_frames.back().it);
  println(color::red, std::format("[line {}]: {}",
                                m_frames.back().chunk_ptr->lines(idx), msg));
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
        push_value(frame->chunk_ptr->constant(next));
      }
      break;
      case op_code::define_var:
      {
        uint32_t next = frame->it.next();
        const std::string& name =
            frame->chunk_ptr->constant(next).as<std::string>();
        m_globals[name] = m_stack.back();
        pop();
      }
      break;
      case op_code::get_var:
      {
        uint32_t next = frame->it.next();
        const std::string& name =
            frame->chunk_ptr->constant(next).as<std::string>();
        push_value(m_globals[name]);
      }
      break;
      case op_code::set_var:
      {
        uint32_t next = frame->it.next();
        const std::string& var =
            frame->chunk_ptr->constant(next).as<std::string>();
        if (m_globals.contains(var))
        {
          m_globals[var] = m_stack.back();
        }
        else
        {
          runtime_error(std::format("Undefined variable '{}'.", var));
        }
        pop();
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
        const std::string& step_name =
            frame->chunk_ptr->constant(frame->it.next()).as<std::string>();
        step_finder finder(
            step_name);
        auto found_step =
            std::find_if(steps().begin(), steps().end(),
                         [&finder](const step& s)
                         {
                           finder.reset_with_next_step(s.definition());
                           return finder.step_matches();
                         });
        if (found_step != steps().end())
        {
          finder.for_each_value([this](const value& v) { push_value(v); });
          found_step->call(finder.values_count(), m_stack.rbegin());
          pop(finder.values_count());
        }
        else
        {
          runtime_error(std::format("Undefined step '{}'", step_name));
          // TODO Error step not found
        }
      }
      break;
      case op_code::step_result:
      {
        pop(2);
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
        pop();
      }
      break;
      case op_code::println:
      {
        println(to_color(frame->it.next()), m_stack.back().as<std::string>());
        pop();
      }
      break;
      case op_code::init_scenario:
      {
        std::cout << "op_code::init_scenario" << std::endl;
      }
      break;
      case op_code::func_return:
      {
        pop();
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