#pragma once

#include <algorithm>

#include "value.hpp"
#include "hooks.hpp"

namespace cuke
{
namespace internal
{

class callback_base
{
 public:
  virtual ~callback_base() = default;
};

template <typename Ret, typename... Args>
class callable : public callback_base
{
 public:
  virtual Ret invoke(Args&&... args) = 0;
};

template <typename Func, typename Ret, typename... Args>
class callable_impl : public callable<Ret, Args&&...>
{
 public:
  explicit callable_impl(Func func) : m_func(std::move(func)) {}

  Ret invoke(Args&&... args) override
  {
    return m_func(std::forward<Args>(args)...);
  }

 private:
  Func m_func;
};

template <typename Hook>
static void run_hook(const std::vector<Hook>& hooks)
{
  std::for_each(hooks.begin(), hooks.end(), [](const auto& h) { h.call(); });
}
template <typename Hook>
static void run_hook(const std::vector<Hook>& hooks,
                     const std::vector<std::string>& tags)
{
  std::for_each(hooks.begin(), hooks.end(),
                [&tags](const auto& h)
                {
                  if (h.valid_tag(tags))
                  {
                    h.call();
                  }
                });
}
class registry
{
 public:
  void sort_steps_by_type()
  {
    sort_steps([](const internal::step& lhs, const internal::step& rhs)
               { return lhs.step_type() < rhs.step_type(); });
  }

  void clear() noexcept
  {
    m_steps.clear();
    m_hooks.before.clear();
    m_hooks.after.clear();
    m_hooks.before_step.clear();
    m_hooks.after_step.clear();
    m_hooks.before_all.clear();
    m_hooks.after_all.clear();
  }

  template <typename Ret, typename... Args, typename Func>
  void push_custom_callback(const std::string& id, Func&& callback)
  {
    using callable_t = callable_impl<std::decay_t<Func>, Ret, Args...>;
    std::cout << "push id " << id << std::endl;
    m_custom_type_callbacks[id] =
        std::make_unique<callable_t>(std::forward<Func>(callback));
  }

  template <typename Ret, typename... Args>
  Ret invoke(const std::string& id, Args&&... args)
  {
    auto it = m_custom_type_callbacks.find(id);
    if (it != m_custom_type_callbacks.end())
    {
      auto* callback = static_cast<callable<Ret, Args&&...>*>(it->second.get());
      if (callback)
      {
        return callback->invoke(std::forward<Args>(args)...);
      }
      else
      {
        // TODO: proper error
        throw std::bad_cast();
      }
    }
    // TODO: proper error
    throw std::runtime_error("Callback not found for ID: " + id);
  }

  void push_step(const internal::step& s) noexcept { m_steps.push_back(s); }
  [[nodiscard]] const std::vector<internal::step>& steps() const noexcept
  {
    return m_steps;
  }

  void push_hook_before(const internal::hook& h) noexcept
  {
    m_hooks.before.push_back(h);
  }

  [[nodiscard]] const std::vector<internal::hook>& hooks_before() const noexcept
  {
    return m_hooks.before;
  }

  void push_hook_after(const internal::hook& h) noexcept
  {
    m_hooks.after.push_back(h);
  }
  [[nodiscard]] const std::vector<internal::hook>& hooks_after() const noexcept
  {
    return m_hooks.after;
  }
  void push_hook_after_all(const internal::hook& h) noexcept
  {
    m_hooks.after_all.push_back(h);
  }
  [[nodiscard]] const std::vector<internal::hook>& hooks_after_all()
      const noexcept
  {
    return m_hooks.after_all;
  }
  void push_hook_before_all(const internal::hook& h) noexcept
  {
    m_hooks.before_all.push_back(h);
  }
  [[nodiscard]] const std::vector<internal::hook>& hooks_before_all()
      const noexcept
  {
    return m_hooks.before_all;
  }
  void push_hook_before_step(const internal::hook& h) noexcept
  {
    m_hooks.before_step.push_back(h);
  }
  [[nodiscard]] const std::vector<internal::hook>& hooks_before_step()
      const noexcept
  {
    return m_hooks.before_step;
  }

  void push_hook_after_step(const internal::hook& h) noexcept
  {
    m_hooks.after_step.push_back(h);
  }
  [[nodiscard]] const std::vector<internal::hook>& hooks_after_step()
      const noexcept
  {
    return m_hooks.after_step;
  }

  void run_hook_before(const std::vector<std::string>& tags) const noexcept
  {
    run_hook(m_hooks.before, tags);
  }
  void run_hook_after(const std::vector<std::string>& tags) const noexcept
  {
    run_hook(m_hooks.after, tags);
  }
  void run_hook_before_step() const noexcept { run_hook(m_hooks.before_step); }
  void run_hook_after_step() const noexcept { run_hook(m_hooks.after_step); }
  void run_hook_before_all() const noexcept { run_hook(m_hooks.before_all); }
  void run_hook_after_all() const noexcept { run_hook(m_hooks.after_all); }

 private:
  template <typename Compare>
  void sort_steps(Compare compare)
  {
    std::sort(m_steps.begin(), m_steps.end(), compare);
  }

  std::vector<internal::step> m_steps;
  struct
  {
    std::vector<internal::hook> before;
    std::vector<internal::hook> after;
    std::vector<internal::hook> before_all;
    std::vector<internal::hook> after_all;
    std::vector<internal::hook> before_step;
    std::vector<internal::hook> after_step;
  } m_hooks;
  std::unordered_map<std::string, std::unique_ptr<callback_base>>
      m_custom_type_callbacks;
};

}  // namespace internal

[[nodiscard]] internal::registry& registry();

}  // namespace cuke
