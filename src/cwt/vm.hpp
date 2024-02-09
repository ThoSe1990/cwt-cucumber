#pragma once

#include <string_view>
#include <unordered_map>

#include "value.hpp"
#include "options.hpp"
namespace cwt::details
{

struct call_frame
{
  chunk* chunk_ptr;
  chunk::const_iterator it;
};

class vm
{
 public:
  vm() = default;
  vm(const options& opts);
  vm(int argc, const char* argv[]);

  [[nodiscard]] return_code run();
  [[nodiscard]] return_code run(
      const std::vector<cwt::details::feature_file>& files);
  [[nodiscard]] return_code interpret(std::string_view source);
  [[nodiscard]] return_code execute_function(function func);
  [[nodiscard]] const std::vector<value>& stack() const;
  [[nodiscard]] const std::vector<call_frame>& frames() const;
  [[nodiscard]] value& global(const std::string& name);

  static void set_options(const options& opts);
  static const options& get_options();

  static void reset();

  static void push_step(const step& s);
  static void push_hook_before(const hook& h);
  static void push_hook_after(const hook& h);
  static void push_hook_before_step(const hook& h);
  static void push_hook_after_step(const hook& h);

  static void current_step_failed();

  const std::unordered_map<return_code, std::size_t> scenario_results() const;
  const std::unordered_map<return_code, std::size_t> step_results() const;

 private:
  [[nodiscard]] static std::vector<step>& steps();
  [[nodiscard]] static std::vector<hook>& before();
  [[nodiscard]] static std::vector<hook>& after();
  [[nodiscard]] static std::vector<hook>& before_step();
  [[nodiscard]] static std::vector<hook>& after_step();

  [[nodiscard]] static std::vector<std::string>& failed_scenarios();
  [[nodiscard]] static std::vector<std::vector<return_code>>& results();

  void push_value(const value& v);
  void pop();
  void pop(std::size_t count);

  void runtime_error(std::string_view msg);
  [[nodiscard]] return_code start();
  void call(const function& func);
  void run_hooks(const std::vector<hook>& hooks, uint32_t n) const;
  [[nodiscard]] return_code final_result() const noexcept;

 private:
  std::vector<value> m_stack;
  std::vector<call_frame> m_frames;
  std::unordered_map<std::string, value> m_globals;
  static options m_options;
  static constexpr const std::size_t m_max_stack_size{256};
};

}  // namespace cwt::details
