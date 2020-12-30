#pragma once
#if defined(MTAO_USE_RANGESv3)
#include <range/v3/view/reverse.hpp>
#else
#include "mtao/iterator/shell.hpp"
#endif
namespace mtao::iterator {
#if defined(MTAO_USE_RANGESv3)
template<typename T>
auto reverse(T &&container) {
    return ranges::view::reverse(container);
}
#else
namespace detail {

    template<typename Type>
    class reverse_container {
      public:
        reverse_container() = delete;
        reverse_container(reverse_container &&) = default;
        reverse_container(const reverse_container &) = delete;
        reverse_container &operator=(reverse_container &&) = delete;
        reverse_container &operator=(const reverse_container &) = delete;
        reverse_container(Type &&t) : m_container(t) {}

        auto begin() const { return m_container.rbegin(); }
        auto end() const { return m_container.rend(); }
        auto begin() { return m_container.rbegin(); }
        auto end() { return m_container.rend(); }

      private:
        choose_storage_t<Type> m_container;
    };
}// namespace detail
template<typename T>
auto reverse(T &&container) {
    return detail::reverse_container<T>(container);
}
#endif
}// namespace mtao::iterator
