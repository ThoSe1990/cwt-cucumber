
#include <iostream>
#include <algorithm>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

extern "C" {
  #include "cucumber_value.h"
}

namespace cuke {
  namespace details {

    class context_type
    {
    public:
        context_type() = default; 

        template<typename T> 
        context_type(T&& value) 
        {
          m_value = std::make_unique<c_model<T>>(std::move(value));
        }

        template<typename T> 
        T& get() 
        {
          return static_cast<c_model<T>*>(m_value.get())->m_value;
        }

    private:   
        struct c_concept 
        {
            virtual ~c_concept() {}
        };

        template<typename T>
        struct c_model : public c_concept 
        {
            c_model(T const& value) 
              : m_value(value) {};
            T m_value;
        };

    private:
        std::unique_ptr<c_concept> m_value;
    };

} // namespace details 

  template <typename T>
  inline std::type_index get_type_id() 
  {
      return std::type_index(typeid(T));
  }

  class scenario_context
  {
    public:
    //TODO another get method: forward values to ctor
      template<typename T>
      T& get() 
      {
        std::cout << "T& get() " << std::endl;
        if (m_data.count(get_type_id<T>()) == 0) {
          m_data[get_type_id<T>()] = T{};
        }
        return m_data[get_type_id<T>()].template get<T>();
      }

      template<typename T, typename... Args>
      T& get(Args&&... args) 
      {
        std::cout << "T& get(Args&&... args) " << std::endl;
        if (m_data.count(get_type_id<T>()) == 0) {
          m_data[get_type_id<T>()] = T{std::forward<Args>(args)...};
        }
        return m_data[get_type_id<T>()].template get<T>();
      }

      void clear() 
      {
        m_data.clear();
      }
    private:
      std::unordered_map<std::type_index, details::context_type> m_data;
  };

  namespace details
  {
    inline scenario_context& get_context()
    {
      static scenario_context sc; 
      return sc;
    }
  
    inline void reset_scenario_context(int, cuke_value*)
    {
      get_context().clear();
    }
  } // namespace details
  
  template<typename T>
  inline T& context()
  {
    return details::get_context().get<T>();
  }
  template<typename T, typename... Args>
  inline T& context(Args&&... args)
  {
    return details::get_context().get<T>(std::forward<Args>(args)...);
  }
} // namespace cuke 
