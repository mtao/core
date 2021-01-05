#pragma once
#include <bitset>
#include <array>
#include <cassert>


namespace mtao::combinatorial {
namespace detail {
    // returns x*(x+1)...(x+n) rather than up to (x-n-1)
    template<bool Rising = true>
    constexpr size_t sequential_product_off_one(size_t x, size_t n) {
        return n == 0 ? 1 : (sequential_product_off_one<Rising>(x, n - 1) * (Rising ? (x + n) : (x - n)));
    }
    // returns x*(x+1)...(x+n-1) if rising
    // returns x*(x-1)...(x-n+1) if not rising rising (hence falling)
    template<bool Rising = true>
    constexpr size_t sequential_product(size_t x, size_t n) {
        return n == 0? 1 : sequential_product_off_one<Rising>(x, n - 1);
    }
}// namespace detail
constexpr size_t factorial(size_t n) {
#ifdef USE_PRECOMPUTED_FACTORIALS
    constexpr std::array<size_t, 10> precomputed{ { 1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880 } };
    return n < 10 ? precomputed[n] : n * factorial(n - 1);
#else
    return n < 1 ? 1 : n * factorial(n - 1);
#endif
}
constexpr size_t factorial_ratio(size_t a, size_t b) {
    //take advantage of precomputed stuff
#ifdef USE_PRECOMPUTED_FACTORIALS
    if (a < 10 && b < 10) {
        return factorial(a) / factorial(b);
    }
#endif
    return a <= b ? 1 : a * factorial_ratio(a - 1, b);
}
constexpr size_t rising_factorial(size_t x, size_t n) {
    return detail::sequential_product<true>(x, n);
}
constexpr size_t falling_factorial(size_t x, size_t n) {
    return detail::sequential_product<false>(x, n);
}
constexpr size_t nCr(size_t n, size_t r) {
    if (n < r) { return 0; }
    if (r > n / 2) {
        r = n - r;
    }
    // TODO: debug rising/falling factorials to see why this isnt right
    // TODO: write tests...
    //return falling_factorial(n,r) / factorial(r);
    return factorial_ratio(n, n - r) / factorial(r);
}
template<size_t N>
constexpr std::bitset<N> nCr_mask(size_t R, size_t L) {
    assert(R <= N);
    assert(L <= nCr(N, R));
    static_assert(N <= 3, "We can't handle more than N=3 right now");
    if (R == 0) { return 0; }
    if (R == N) { return (1 << N) - 1; }
    if (R == 1) { return 1 << L; }
    if constexpr (N == 3) {
        if (R == 2) { return (1 << N) - 1 - (1 << L); }
    }
    return 0;
}
template<size_t N>
constexpr size_t nCr_unmask(size_t R, const std::bitset<N> &bs) {
    assert(R <= N);
    static_assert(N <= 3, "We can't handle more than N=3 right now");
    if (R == 0) { return 0; }
    if (R == N) { return 0; }
    if (R == 1) {
        assert(bs.count() == 1);
        for (size_t i = 0; i < N; ++i) {
            if (bs[i]) {
                return i;
            }
        }
    }
    if constexpr (N == 3) {
        if (R == 2) {
            assert(bs.count() == N - 1);
            for (size_t i = 0; i < N; ++i) {
                if (!bs[i]) return i;
            }
        }
    }
    return 0;
}
template<size_t N, size_t R>
constexpr auto nCr_mask(size_t L) {
    static_assert(R <= N);
    static_assert(N <= 3, "We can't handle more than N=3 right now");
    return nCr_mask<N>(R, L);
}
template<size_t N, size_t R>
constexpr auto nCr_unmask(const std::bitset<N> L) {
    static_assert(R <= N);
    static_assert(N <= 3, "We can't handle more than N=3 right now");
    return nCr_unmask<N>(R, L);
}
template<size_t N, size_t R, size_t L>
constexpr auto nCr_mask() {
    static_assert(L < nCr(N, R));
    return nCr_mask<N, R>(L);
}
namespace internal {
    template<size_t... N>
    constexpr auto nCr(std::integer_sequence<size_t, N...>) {
        return std::integer_sequence<size_t, combinatorial::nCr(sizeof...(N) - 1, N)...>();
    }
}// namespace internal
template<size_t D>
constexpr auto nCr() {
    return internal::nCr(std::make_integer_sequence<size_t, D + 1>());
}
}// namespace mtao::combinatorial
