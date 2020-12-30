#pragma once
#include <memory>
#if defined(MTAO_USE_RANGESv3)
#include <ranges>
#endif
#include <tuple>
#include <utility>

namespace mtao::iterator {
#if !defined(MTAO_USE_RANGESv3)
namespace detail {
    template<typename BeginIt, typename EndIt>
    struct shell_container;

    template<typename T, bool Expr = std::is_lvalue_reference_v<T>>
    struct choose_storage {
        using value_type = T &;
    };
    template<typename T>
    struct choose_storage<T, false> {
        using value_type = T;
    };
    template<typename T, int D>
    struct choose_storage<T[D], true> {
        using value_type = detail::shell_container<T *, T *>;
    };
    template<typename T>
    struct choose_storage<const T, false> {
        using value_type = const T;
    };
    template<typename T>
    struct choose_storage<const T, true> {
        using value_type = const T &;
    };
    template<typename T>
    using choose_storage_t = typename choose_storage<T>::value_type;

    template<typename BeginIt, typename EndIt>
    struct shell_container {
      public:
        using iterator = BeginIt;
        shell_container(BeginIt &&b, EndIt &&e) : m_begin(b), m_end(e) {}
        auto begin() { return m_begin; }
        auto begin() const { return m_begin; }
        auto end() { return m_end; }
        auto end() const { return m_end; }

      private:
        const BeginIt m_begin;
        const EndIt m_end;
    };
}// namespace detail
#endif

template<typename BeginIt, typename EndIt>
auto shell(BeginIt &&b, EndIt &&e) {
#if defined(MTAO_USE_RANGESv3)
#else
    return detail::shell_container<BeginIt, EndIt>(std::forward<BeginIt>(b),
                                                   std::forward<EndIt>(e));

#endif
}
template<typename Container>
auto shell(Container &&c) {
    return shell(c.begin(), c.end());
}
template<typename T, int D>
auto shell(const T c[D]) {
    return shell(c, c + D);
}
template<typename T, int D>
auto shell(T c[D]) {
    return shell(c, c + D);
}
template<typename T>
auto shell(T *c, int D) {
    return shell(c, c + D);
}
template<typename T>
auto shell(const T *c, int D) {
    return shell(c, c + D);
}
template<typename T, typename Deleter>
auto shell(std::unique_ptr<T[], Deleter> &c, int D) {
    return shell(c.get(), D);
}
template<typename T, typename Deleter>
auto shell(const std::unique_ptr<T[], Deleter> &c, int D) {
    return shell(c.get(), D);
}
}// namespace mtao::iterator
