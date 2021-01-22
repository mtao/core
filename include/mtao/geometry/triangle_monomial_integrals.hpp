#pragma once
#include <spdlog/spdlog.h>
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
// template <typename T>
// std::vector<T> triangle_monomial_integrals(size_t max_dim) {
//    // max dim + 1 dimensiosn will be used
//    std::vector<T> ret;
//    size_t max_integral_deg = max_dim + 3;
//    ret.reserve(((max_integral_deg + 1) * (max_integral_deg + 2)) / 2);
//    ;
//    std::vector<size_t> pascal_row;
//
//    pascal_row.emplace_back(1);
//    for (size_t d = 1; d < max_integral_deg; ++d) {
//        std::vector<size_t> new_pascal_row(pascal_row.size() + 1);
//        new_pascal_row.front() = 1;
//        new_pascal_row.back() = 1;
//        for (size_t j = 0; j < pascal_row.size() - 1; ++j) {
//            new_pascal_row[j + 1] = pascal_row[j] + pascal_row[j + 1];
//        }
//
//        // for(auto&& idx: new_pascal_row) {
//        //    std::cout << idx << " ";
//        //}
//        // std::cout << std::endl;
//        if (d >= 2) {
//            // j + k + 3 == d
//            // j in [0,d-3]
//            // jp1 in [1,d-2]
//            // std::cout << "Monomials of max deg " << d-2 << std::endl;
//            double val = d-1;
//            for (size_t j = 0; j < d - 1; ++j) {
//                // std::cout << "x^" << j << " y^" << d-2-j << " => ";
//                // std::cout << (1. / new_pascal_row[j+1]) << std::endl;
//                ret.emplace_back(1. / (new_pascal_row[j + 1] * val));
//            }
//        }
//
//        pascal_row = std::move(new_pascal_row);
//    }
//    return ret;
//}
template<typename T, typename Derived1, typename Derived2, typename Derived3>
std::vector<T> triangle_monomial_integrals(
  size_t max_dim,
  const Eigen::MatrixBase<Derived1> &A,
  const Eigen::MatrixBase<Derived2> &B,
  const Eigen::MatrixBase<Derived3> &C) {

    constexpr static int Dim = Derived1::RowsAtCompileTime;
    if constexpr (Dim == 2) {
        const auto &a = A;
        const auto &b = B;
        const auto &c = C;
        // max dim + 1 dimensiosn will be used
        std::vector<T> ret;
        size_t max_integral_deg = max_dim + 5;
        algebra::PascalTriangle pt(max_integral_deg);
        ret.reserve(((max_integral_deg + 1) * (max_integral_deg + 2)) / 2);
        ;
        auto u = (b - a).eval();
        auto v = (c - a).eval();
        T det;

        if constexpr (Dim == 2) {
            det = u.homogeneous().cross(v.homogeneous()).z();
        } else {
            det = u.cross(v).norm();
        }
        for (size_t d = 0; d <= max_dim; ++d) {
            //std::cout << "<<<<<<<<<<<<" << d << std::endl;
            // int off = (d * (d + 1)) / 2;
            // for (size_t j = 0; j <= d; ++j) {
            for (long long j = d; j >= 0; --j) {
                //std::cout << "j = " << j << " => " << std::endl;
                //;
                // monomial is
                // (s u.x + t v.x + a.x)^j + (s u.y + t v.y + a.y)^k
                // j+k = d
                // 0+d = d => (s u.x + t v.x + a.x)^d
                // d+0 = d => (s u.y + t v.y + a.y)^d

                // if dim == 3 we have to do this t hird loop
                //for (long long k = (d - j); k >= (long long)(d - j) ; --k) {

                for (long long k = (Dim == 2) ? (d - j) : (d - j - 1); k >= (long long)((Dim == 2) ? (d - j) : 0); --k) {
                    //for (size_t k = (Dim == 2) ? (d - j) : 0; k <= d - j; ++k) {
                    //std::cout << "k = " << k << " => " << std::endl;
                    double val = 0.0;
                    // term of (s u.x + t v.x + a.x)^j expanded
                    for (size_t m = 0; (long long)m <= j; ++m) {
                        // =  j \choose m  (a bunch of stuff)
                        for (size_t o = 0; o <= j - m; ++o) {
                            // holding s to be constant, the different ways t v.x
                            // and a.x can happen (j \choose m) (s u.x)^m (t v.x)^o
                            // a.x^(j - m - o)

                            T jom_coeff = pt(j, m) * pt(j - m, o) * std::pow<T>(u.x(), m) * std::pow<T>(v.x(), o) * std::pow<T>(a.x(), j - m - o);

                            // term of (s u.y + t v.y + a.y)^k expanded
                            for (size_t n = 0; (long long)n <= k; ++n) {
                                // =  k \choose n  (a bunch of stuff)
                                for (size_t p = 0; p <= k - n; ++p) {
                                    // holding s to be constant, the different ways
                                    // t v.y and a.y can happen (k \choose n) {k-n
                                    // \choose p} (s u.y)^n (t v.y)^p a.y^(k - n -
                                    // p)
                                    T knp_coeff = pt(k, n) * pt(k - n, p) * std::pow<T>(u.y(), n) * std::pow<T>(v.y(), p) * std::pow<T>(a.y(), k - n - p);

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
                                        T v = 1.0 / ((m + n + 1.) * (m + n + o + p + 2.) * pt(n + m + 1 + o + p, o + p));
                                        val += v * jom_coeff * knp_coeff;
                                    } else {
                                        // term of (s u.y + t v.y + a.y)^k expanded
                                        for (size_t l = 0; l <= d - j - k; ++l) {
                                            for (size_t q = 0; q <= l; ++q) {
                                                // =  k \choose n  (a bunch of
                                                // stuff)
                                                for (size_t r = 0; r <= l - q;
                                                     ++r) {
                                                    // holding s to be constant, the
                                                    // different ways t v.y and a.y
                                                    // can happen (k \choose n) {k-n
                                                    // \choose p} (s u.y)^n (t
                                                    // v.y)^p a.y^(k - n - p)
                                                    T lqr_coeff =
                                                      pt(l, q) * pt(l - q, r) * std::pow<T>(u.z(), q) * std::pow<T>(v.z(), r) * std::pow<T>(a.z(), l - q - r);
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
                                                    T v = 1.0 / ((m + n + l + 1.) * (m + n + l + o + p + r + 2.) * pt(n + m + l + 1 + o + p + r, o + p + r));
                                                    val += v * jom_coeff * knp_coeff * lqr_coeff;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        // std::cout << "Value: " << val << std::endl;
                    }
                    //std::cout << "Value: " << val << std::endl;
                    ret.emplace_back(val * det);
                }
            }
        }
        return ret;
    } else {
        std::vector<T> ret;
        // gotta check why 5 was put here
        size_t max_integral_deg = 2 * max_dim + 2;
        algebra::PascalTriangle pt(max_integral_deg);
        ret.reserve(((max_integral_deg + 1) * (max_integral_deg + 2)) / 2);
        auto U = (B - A).eval();
        auto V = (C - A).eval();
        T det = U.cross(V).norm();

        //std::cout << "U: " << U.transpose() << std::endl;
        //std::cout << "V: " << V.transpose() << std::endl;
        //std::cout << "A: " << A.transpose() << std::endl;

        for (int d = 0; d <= int(max_dim); ++d) {
            mtao::Vec3i exps;
            int &a = exps(0);
            int &b = exps(1);
            int &c = exps(2);

            algebra::PascalTriangle pt(max_integral_deg);

            auto get_vec_combination = [&pt](auto &&a, auto &&b) -> T {
                return a.binaryExpr(b, [&pt](auto a, auto b) -> T { return pt(a, b); }).prod();
            };
            for (a = d; a >= 0; --a) {

                const int arem = d - a;
                for (b = arem; b >= 0; --b) {
                    c = arem - b;

                    //std::cout << "Trying: " << exps.transpose() << std::endl;
                    double value = 0;

                    mtao::Vec3i u_exps;
                    int &ua = u_exps(0);
                    int &ub = u_exps(1);
                    int &uc = u_exps(2);

                    mtao::Vec3i v_exps;
                    int &va = v_exps(0);
                    int &vb = v_exps(1);
                    int &vc = v_exps(2);

                    mtao::Vec3i w_exps;
                    for (ua = 0; ua <= a; ++ua) {
                        for (ub = 0; ub <= b; ++ub) {
                            for (uc = 0; uc <= c; ++uc) {
                                T uterm = U.array().pow(u_exps.template cast<T>().array()).prod() * get_vec_combination(exps, u_exps);
                                mtao::Vec3i urem = exps - u_exps;
                                int usum = u_exps.sum();

                                for (va = 0; va <= urem.x(); ++va) {
                                    for (vb = 0; vb <= urem.y(); ++vb) {
                                        for (vc = 0; vc <= urem.z(); ++vc) {
                                            T vterm = V.array().pow(v_exps.template cast<T>().array()).prod() * get_vec_combination(urem, v_exps);
                                            mtao::Vec3i vrem = urem - v_exps;
                                            int vsum = v_exps.sum();

                                            w_exps = vrem;
                                            {
                                                {
                                                    {
                                                        //for (wa = 0; wa <= vrem.x(); ++wa) {
                                                        //    for (wb = 0; wb <= vrem.y(); ++wb) {
                                                        //        for (wc = 0; wc <= vrem.z(); ++wc) {
                                                        //mtao::Vec3i wrem = vrem - w_exps;
                                                        T wterm = A.array().pow(w_exps.template cast<T>().array()).prod() * get_vec_combination(vrem, w_exps);
                                                        //int wsum = w_exps.sum();


                                                        // now we have the precise polynomial that we want to look at determined
                                                        //std::cout << "U^(" << u_exps.transpose() << ") ";
                                                        //std::cout << "V^(" << v_exps.transpose() << ") ";
                                                        //std::cout << "C^(" << w_exps.transpose() << ") ";

                                                        //std::cout << " = " << uterm << " * " << vterm << " * " << wterm;
                                                        //std::cout << " = " << uterm * vterm * wterm << std::endl;
                                                        //T f0 = T(1.0) / (usum + 1) * (T(1.0) / (vsum + 1) - T(1.0) / (wsum + vsum + 2));
                                                        //T f1 = T(1.0) / ((vsum + 1) * (usum + vsum + 2));
                                                        //T f2 = T(1.0) / ((usum + vsum + wsum + 3) * (vsum * wsum + 2));

                                                        //spdlog::info("J/K limits: {} {}; denoms min {} {}", wsum + 1, vsum + wsum + 2, vsum + wsum + 2, vsum + wsum + usum + 3);
                                                        T base = uterm * vterm * wterm / (vsum + 1);
                                                        for (int j = 0; j <= vsum + 1; ++j) {

                                                            T jterm = T(pt(vsum + 1, j)) / (usum + j + 1);
                                                            T val = jterm * base;
                                                            //spdlog::info("Adding {} * {} = {}", base, jterm, val);
                                                            value += val * (j % 2 == 0 ? 1 : -1);
                                                        }
                                                        //T I = f0 - f1 + f2;
                                                        //std::cout << "I Term:" << f0 << " - " << f1 << " + " << f2 << " = " << I << std::endl;
                                                        //spdlog::info("Adding {} {} {} {} = {}", uterm, vterm, wterm, I, uterm * vterm * wterm * I);
                                                        //value += uterm * vterm * wterm * I;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    //std::cout << "p^(" << exps.transpose() << ") = " << (value * det) << std::endl;
                    std::cout << std::endl;
                    ret.emplace_back(value * det);
                }
            }
        }
        //std::cout << "Value: " << val << std::endl;
        return ret;
    }
}
}// namespace mtao::geometry
