#pragma once
#include <mtao/types.hpp>
#include <vector>

#include "mtao/algebra/pascal_triangle.hpp"

namespace mtao::geometry {
// returns a vector of monomial integrals
// \int_{0}^1 \int_{0}^{1-x} x^j y^k dy dx
// = (j+1)! (k+2)! / (j+k+3)!
// = 1 / ( N choose J ) for J = j+1, N = K+1 )
// returns a linear vector [ M1 M2 M3 M4 ]
// where |Mk| = k, Mk = [ x^k x^{k-1}y ... y^k ]
template <typename T, typename Derived1, typename Derived2>
std::vector<T> edge_monomial_integrals(size_t max_dim,
                                       const Eigen::MatrixBase<Derived1>& a,
                                       const Eigen::MatrixBase<Derived2>& b) {
    // max dim + 1 dimensiosn will be used
    std::vector<T> ret;
    size_t max_integral_deg = max_dim + 1;
    algebra::PascalTriangle pt(max_integral_deg);
    ret.reserve(max_integral_deg + 1);
    auto u = (b - a).eval();
    double len = u.norm();

    constexpr static int Dim = Derived1::RowsAtCompileTime;
    for (size_t d = 0; d <= max_dim; ++d) {
        // std::cout << "<<<<<<<<<<<<" << d << std::endl;
        // int off = (d * (d + 1)) / 2;
        // for (size_t j = 0; j <= d; ++j) {
        for (long long j = d; j >= 0; --j) {
            // std::cout << "j = " << j << " => " << std::endl;
            //;
            // monomial is
            // (s u.x + a.x)^j + (s u.y  + a.y)^k
            // j+k = d
            // 0+d = d => (s u.x + a.x)^d
            // d+0 = d => (s u.y + a.y)^d

            // if dim == 3 we have to do this t hird loop
            for (long long k = (Dim == 2) ? (d - j) : (d - j - 1);
                 k >= (long long)((Dim == 2) ? (d - j) : 0); --k) {
                double val = 0.0;
                // term of (s u.x  + a.x)^j expanded
                for (size_t m = 0; (long long)m <= j; ++m) {
                    // =  j \choose m  (a bunch of stuff)
                    // holding s to be constant, the different ways
                    // and a.x can happen (j \choose m) (s u.x)^m
                    // a.x^(j - m)
                    T jm_coeff = pt(j, m) * std::pow<T>(u.x(), m) *
                                 std::pow<T>(a.x(), j - m);
                    // term of (s u.y + a.y)^k expanded
                    for (size_t n = 0; (long long)n <= k; ++n) {
                        // =  k \choose n  (a bunch of stuff)
                        // holding s to be constant, the different ways
                        // t v.y and a.y can happen (k \choose n) {k-n
                        // \choose p} (s u.y)^n a.y^(k - n)
                        T kn_coeff = pt(k, n) * std::pow<T>(u.y(), n) *
                                     std::pow<T>(a.y(), k - n);

                        // so we have to integrate
                        // (j \choose m){j-m \choose o} (u.x)^m (v.x)^o
                        // a.x^(j - m - o) (k \choose n){k-n \choose p}
                        // (u.y)^n (v.y)^p a.y^(k - n - p) s^{m+n}
                        // t^{o+p}

                        // \int_0^1 \int_0^{1-t} s^{m+n} t^{o+p} ds dt
                        // \int_0^1  \frac{1}{m+n+1} (1-t)^{m+n+1}
                        // t^{o+p} dt Well \frac{1}{N+1} = \int_0^1 {N
                        // \choose m} x^m x^{N-m} dx Well \frac{1}{(N+1)
                        // {N \choose m}} = \int_0^1 x^m x^{N-m} dx so
                        //\frac{1}{(m+n+1)(m+n+o+p+2){o+p \choose
                        // n+m+1+o+p}

                        if constexpr (Dim == 2) {
                            T v = 1.0 / (m + n + 1.);
                            val += v * jm_coeff * kn_coeff;
                        } else {
                            // term of (s u.y + a.y)^k expanded
                            for (size_t l = 0; l <= d - j - k; ++l) {
                                for (size_t q = 0; q <= l; ++q) {
                                    // =  k \choose n  (a bunch of
                                    // stuff)
                                    // holding s to be constant, the
                                    // different ways t v.y and a.y
                                    // can happen (k \choose n) {k-n
                                    // \choose p} (s u.y)^n (t
                                    // v.y)^p a.y^(k - n - p)
                                    T lq_coeff = pt(l, q) *
                                                 std::pow<T>(u.z(), q) *
                                                 std::pow<T>(a.z(), l - q);
                                    // (j \choose m){j-m \choose o}
                                    // (u.x)^m (v.x)^o a.x^(j - m -
                                    // o) (k \choose n){k-n \choose
                                    // p} (u.y)^n (v.y)^p a.y^(k - n
                                    // - p) (l \choose q){l-q
                                    // \choose r} (u.z)^l (v.z)^r
                                    // a.z^(l-q-r) s^{m+n+l}
                                    // t^{o+p+r}
                                    //
                                    // \int_0^1 \int_0^{1-t}
                                    // s^{m+n+l} t^{o+p+r} ds dt
                                    // \int_0^1  \frac{1}{m+n+l+1}
                                    // (1-t)^{m+n+l+1} t^{o+p} dt
                                    //\frac{1}{(m+n+l+1)(m+n+l+o+p+r+2){o+p+r
                                    //\choose n+m+l+1+o+p+r}
                                    T v = 1.0 / (m + n + l + 1.);
                                    val += v * jm_coeff * kn_coeff * lq_coeff;
                                }
                            }
                        }
                    }
                }
                // std::cout << "Value: " << val << std::endl;
                ret.emplace_back(val * len);
            }
        }
    }
    return ret;
}
}  // namespace mtao::geometry
