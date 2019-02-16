#pragma once
#include <bitset>
#include <array>
#include <cassert>


namespace mtao {
    namespace combinatorial {
        constexpr size_t factorial(size_t n) {
#ifdef USE_PRECOMPUTED_FACTORIALS
            constexpr std::array<size_t,10> precomputed{{1,1,2,6,24,120,720,5040,40320,362880}};
            return n<10?precomputed[n]:n*factorial(n-1);
#else
            return n<1?1:n*factorial(n-1);
#endif
        }
        constexpr size_t factorial_ratio(size_t a,size_t b) {
            //take advantage of precomputed stuff
#ifdef USE_PRECOMPUTED_FACTORIALS
            if(a < 10 && b < 10) {
                return factorial(a)/factorial(b);
            }
#endif
            return a<=b?1:a*factorial_ratio(a-1,b);
        }
        constexpr size_t nCr(size_t n, size_t r) {
            if (n < r) { return 0; }
            if(r > n/2) {
                r = n - r;
            }
            return factorial_ratio(n,n-r) / factorial(r);
        }
        template <size_t N>
            constexpr std::bitset<N> nCr_mask(size_t R, size_t L) {
                assert(R <= N);
                assert(L <= nCr(N,R));
                static_assert(N <= 3, "We can't handle more than N=3 right now");
                if(R == 0) { return 0; }
                if(R == N) { return (1<<N)-1; }
                if(R == 1) { return 1<<L;}
                if constexpr(N == 3) {
                    if(R == 2) { return (1<<N)-1-(1<<L); }
                }
                return 0;
            }
        template <size_t N, size_t R>
            constexpr auto nCr_mask(size_t L) {
                static_assert(R <= N);
                static_assert(N <= 3, "We can't handle more than N=3 right now");
                return nCr_mask<N>(R,L);
            }
        template <size_t N, size_t R, size_t L>
            constexpr auto nCr_mask() {
                static_assert(L < nCr(N,R));
                return nCr_mask<N,R>(L);
            }
        namespace internal {
            template <size_t... N>
                constexpr auto nCr(std::integer_sequence<size_t,N...>) {
                    return std::integer_sequence<size_t,combinatorial::nCr(sizeof...(N)-1,N)...>();
                }
        }
        template <size_t D>
            constexpr auto nCr() {
                return internal::nCr(std::make_integer_sequence<size_t,D+1>());
            }
    }
}
