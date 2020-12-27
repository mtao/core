#pragma once
#include <type_traits>
#include <utility>
#define MTAO_ACCESSOR(T, NAME, MEMBER) \
    inline T NAME() { return MEMBER; }

#define MTAO_ACCESSOR_CONST(T, NAME, MEMBER) \
    inline T NAME() const { return MEMBER; }

#define MTAO_ACCESSORS(T, NAME, MEMBER) \
    MTAO_ACCESSOR(T&, NAME, MEMBER)     \
    MTAO_ACCESSOR_CONST(const T&, NAME, MEMBER)

#define MTAO_ACCESSOR_PROTOTYPES(T, NAME) \
    T& NAME();                            \
    const T& NAME() const;

#define MTAO_ACCESSOR_IMPL(T, CLASS, NAME, MEMBER) \
    auto CLASS::NAME()->T& { return MEMBER; }      \
    auto CLASS::NAME() const->const T& { return MEMBER; }

#define MTAO_GETSET_ACCESSOR(T, NAME, MEMBER) \
    T& get_##NAME() const { return MEMBER; }  \
    void set_##NAME(const T& v) { MEMBER = v; }
namespace mtao {
template <typename T>
T clamp(const T& value, const T& a, const T& b) {
    if (value < a) {
        return a;
    } else if (value > b) {
        return b;
    } else {
        return value;
    }
}
namespace internal {
constexpr int recursive_pow(int a, int b) {
    if (b == 0) {
        return 1;
    }
    return a * recursive_pow(a, b - 1);
}
}  // namespace internal
template <int A, int B,
          typename = typename std::enable_if<(B >= 0), std::nullptr_t>::type>
struct pow {
    constexpr static int value = internal::recursive_pow(A, B);
};

namespace detail {
template <int... N>
int product(std::integer_sequence<int, N...>) {
    return (N * ... * 1);
}

template <int... N>
int sum(std::integer_sequence<int, N...>) {
    return (N + ... + 1);
}

}  // namespace detail

template <int N>
constexpr int factorial() {
    return detail::product(std::make_integer_sequence<int, N>());
}
constexpr int factorial(int N) { return (N > 1) ? (N * factorial(N - 1)) : 1; }

/*
template <typename ContainerType>
    constexpr size_t size(const ContainerType& c) {
        return c.size();
    }
    */

}  // namespace mtao
