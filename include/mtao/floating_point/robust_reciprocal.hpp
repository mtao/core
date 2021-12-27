#pragma once
#include <concepts>
#include <cmath>
#include <type_traits>


namespace mtao::floating_point {

    // 


// Accelerating Correctly Rounded Floating-Point Division When the Divisor is Known in Advance
// N. Brisebarre, J.-M. Muller and S. K. Raina
//http://perso.ens-lyon.fr/jean-michel.muller/fpdiv.html
template<std::floating_point T>
struct RobustReciprocal_1fma_1prod {
    T h;
    T l;
    // although std::fma doesn't seem to be constexpr, but at least with -O2 it does get consteval'd
    constexpr RobustReciprocal_1fma_1prod(T v) : h(T{ 1.0 } / v), l(std::fma(h, v, T{ -1.0 })) {
    }
    constexpr T eval(T x) const {
        return std::fma(x, h, x * l);
    }
};

// deduction guide to make it easier
template<std::floating_point T>
RobustReciprocal_1fma_1prod(T) -> RobustReciprocal_1fma_1prod<T>;

constexpr auto robust_quotient(std::floating_point auto a, std::floating_point auto b) 
{
    RobustReciprocal_1fma_1prod recip(b);
    return recip.eval(a);
}

}// namespace mtao::floating_point
