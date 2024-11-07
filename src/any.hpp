#pragma once 

#include <memory>

namespace cuke::internal {

class any_base
{
 public:
  virtual ~any_base() = default;
  virtual std::unique_ptr<any_base> clone() const = 0;
};

template <typename T>
class any_impl : public any_base
{
 public:
  any_impl(T value) : m_value(std::move(value)) {}

  std::unique_ptr<any_base> clone() const override
  {
    return std::make_unique<any_impl<T>>(m_value);
  }

  T get() const { return m_value; }

  template <typename U>
  U convert() const
  {
    return static_cast<U>(m_value);
  }

 private:
  T m_value;
};

class any
{
 public:
  template <typename T>
  any(T value) : m_data(std::make_unique<any_impl<T>>(std::move(value)))
  {
  }

  any(const any& other) : m_data(other.m_data->clone()) {}

  template <typename T>
  operator T() const
  {
    auto ptr = static_cast<any_impl<T>*>(m_data.get());
    if (!ptr)
    {
      // TODO: proper error 
      throw std::runtime_error("Invalid cast.");
    }
    return ptr->get();
  }

 private:
  std::unique_ptr<any_base> m_data;
};

} // namespace cuke::internal
