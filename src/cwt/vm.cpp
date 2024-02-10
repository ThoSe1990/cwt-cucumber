#include <format>
#include <fstream>
#include <algorithm>

#include "vm.hpp"
#include "util.hpp"
#include "chunk.hpp"
#include "hooks.hpp"
#include "context.hpp"
#include "options.hpp"
#include "step_finder.hpp"
#include "compiler/cucumber.hpp"

#ifdef PRINT_STACK
#include "debug.hpp"
#endif

namespace cwt::details
{
options vm::m_options{};

vm::vm(const options& opts)  // : m_options(opts)
{
  m_options = opts;
}
vm::vm(int argc, const char* argv[])
{
  terminal_arguments terminal_args(argc, argv);
  m_options = terminal_args.get_options();
  m_files = terminal_args.get_files();
}

return_code vm::run()
{
  return_code result = return_code::passed;
  for (const auto& file : m_files)
  {
    std::ifstream in(file.path);
    std::string script((std::istreambuf_iterator<char>(in)),
                       std::istreambuf_iterator<char>());

    return_code current = run(script);
    if (current != return_code::passed)
    {
      result = return_code::failed;
    }
  }
  return result;
}
void vm::set_options(const options& opts) { m_options = opts; }
const options& vm::get_options() { return m_options; }
return_code vm::run(std::string_view source)
{
  compiler::cucumber c(source);
  c.set_options(m_options);
  c.compile();

  if (c.no_error())
  {
    return execute_function(c.make_function());
  }
  else
  {
    return return_code::compile_error;
  }
}
const std::unordered_map<return_code, std::size_t> vm::scenario_results() const
{
  return get_scenario_results(results());
}
const std::unordered_map<return_code, std::size_t> vm::step_results() const
{
  return get_step_results(results());
}
return_code vm::final_result() const noexcept
{
  println();
  println();

  if (failed_scenarios().size() > 0)
  {
    println(color::red, "Failed Scenarios: ");
    for (const std::string& failed : failed_scenarios())
    {
      println(color::red, std::format("  {}", failed));
    }
    println();
  }

  std::unordered_map<return_code, std::size_t> scenarios =
      get_scenario_results(results());
  print(std::format("{} Scenarios", count_values(scenarios)));
  print(scenarios);

  std::unordered_map<return_code, std::size_t> steps =
      get_step_results(results());
  print(std::format("{} Steps", count_values(steps)));
  print(steps);

  // TODO Failed Scenarios:

  return to_return_code(scenarios);
}

return_code vm::execute_function(function func)
{
  // FYI a call frame stores the current frame as raw pointer,
  //  pointing to the chunk_ptr / function on the stack
  //  if the vector reallocates when capacity is exceeded
  //  we loose the rawpointer to the chunk in the current call frame
  //  for now the stack is under control and will most likely not m_exceed
  //  max_stack_size only if a step has more than ~250 variables which is rather
  //  unlikely
  m_stack.reserve(m_max_stack_size);

  push_value(std::move(func));
  call(m_stack.back().as<function>());
  return start();
}

const std::vector<value>& vm::stack() const { return m_stack; }
const std::vector<call_frame>& vm::frames() const { return m_frames; }

value& vm::global(const std::string& name)
{
  if (m_globals.contains(name)) [[likely]]
  {
    return m_globals[name];
  }
  else [[unlikely]]
  {
    throw std::runtime_error(std::format("Undefined variable '{}'", name));
  }
}

void vm::push_value(const value& v)
{
  if (m_stack.size() < m_max_stack_size)
  {
    m_stack.push_back(v);
  }
  else
  {
    runtime_error("Stack Overflow: Maximum stack size reached.");
  }
}
void vm::pop() { m_stack.pop_back(); }
void vm::pop(std::size_t count)
{
  for (int i = 0; i < count; ++i)
  {
    m_stack.pop_back();
  }
}
// TODO call reset in destructor? 
void vm::reset()
{
  steps().clear();
  before().clear();
  after().clear();
  before_step().clear();
  after_step().clear();
  results().clear();
  reset_context();
  failed_scenarios().clear();
  m_options = options{};
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
std::vector<std::string>& vm::failed_scenarios()
{
  static std::vector<std::string> instance;
  return instance;
}
void vm::push_hook_after_step(const hook& h) { after_step().push_back(h); }

std::vector<std::vector<return_code>>& vm::results()
{
  static std::vector<std::vector<return_code>> instance;
  return instance;
}
void vm::current_step_failed()
{
  results().back().back() = return_code::failed;
}

void vm::runtime_error(std::string_view msg)
{
  std::size_t idx = m_frames.back().chunk_ptr->get_index(m_frames.back().it);
  println(color::red, std::format("[line {}]: {}",
                                  m_frames.back().chunk_ptr->lines(idx), msg));
}

void vm::run_hooks(const std::vector<hook>& hooks, uint32_t tag_count) const
{
  for (const auto& h : hooks)
  {
    if (h.valid_tags(tag_count, m_stack.rbegin()))
    {
      h.call();
    }
  }
}

void vm::call(const function& func)
{
  m_frames.push_back(call_frame{func.get(), func->cbegin()});
}

return_code vm::start()
{
  call_frame* frame = &m_frames.back();
  while (frame->it != frame->chunk_ptr->cend())
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
      case op_code::hook_before:
      {
        uint32_t tag_count = frame->it.next();
        run_hooks(before(), tag_count);
        pop(tag_count);
      }
      break;
      case op_code::hook_after:
      {
        uint32_t tag_count = frame->it.next();
        run_hooks(after(), tag_count);
        pop(tag_count);
      }
      break;
      case op_code::hook_before_step:
      {
        run_hooks(before_step(), 0);
      }
      break;
      case op_code::hook_after_step:
      {
        run_hooks(after_step(), 0);
      }
      break;
      case op_code::reset_context:
      {
        reset_context();
      }
      break;
      case op_code::call_step:
      {
        const std::string& step_name =
            frame->chunk_ptr->constant(frame->it.next()).as<std::string>();
        step_finder finder(step_name);
        auto found_step =
            std::find_if(steps().begin(), steps().end(),
                         [&finder](const step& s)
                         {
                           finder.reset_with_next_step(s.definition());
                           return finder.step_matches();
                         });
        if (found_step != steps().end())
        {
          results().back().push_back(return_code::passed);
          finder.for_each_value(
              [this](const value& v)
              {
                if (v.type() != value_type::nil)
                {
                  push_value(v);
                }
              });
          found_step->call(finder.values_count(), m_stack.rbegin());
          pop(finder.values_count());
        }
        else
        {
          results().back().push_back(return_code::undefined);
          runtime_error(std::format("Undefined step '{}'", step_name));
        }

        if (results().back().back() != return_code::passed)
        {
          failed_scenarios().push_back(frame->chunk_ptr->name());
        }
      }
      break;
      case op_code::call:
      {
        uint32_t argc = frame->it.next();  // this is arg count
        call(m_stack.back().as<function>());
        frame = &m_frames.back();
      }
      break;
      case op_code::print_step_result:
      {
        std::string file_line = m_stack.back().copy_as<std::string>();
        pop();
        std::string step = m_stack.back().copy_as<std::string>();
        pop();
        const return_code step_result = results().back().back();
        print(to_color(step_result),
              std::format("{} {}", step_prefix(step_result), step));
        print("  ");
        println(color::black, file_line);
      }
      break;
      case op_code::jump_if_failed:
      {
        uint32_t target = frame->it.next();
        if (!results().back().empty() &&
            results().back().back() != return_code::passed)
        {
          results().back().push_back(return_code::skipped);
          // TODO if compiler already has the target as difference
          // we don't need to calculate this at runtime
          frame->it += target - frame->chunk_ptr->get_index(frame->it);
        }
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
      case op_code::print_linebreak:
      {
        println();
      }
      break;
      case op_code::print_indent:
      {
        print("  ");
      }
      break;
      case op_code::init_scenario:
      {
        results().push_back({});
      }
      break;
      case op_code::func_return:
      {
        pop();
        m_frames.pop_back();
        if (m_frames.empty())
        {
          return final_result();
        }
        else
        {
          frame = &m_frames.back();
        }
      }
      break;
      default:
      {
        println(color::red,
                std::format("Cuke VM: Missing instruction '{}'", current));
        return return_code::runtime_error;
      }
    }
  }

  println(color::red, "Cuke VM: Call frame out of range.");
  return return_code::runtime_error;
}

}  // namespace cwt::details