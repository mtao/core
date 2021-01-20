#pragma once
#include <iostream>
#include <spdlog/spdlog.h>
#include <mtao/types.hpp>
#include <vector>

#include "mtao/algebra/pascal_triangle.hpp"

namespace mtao::geometry {

template<typename T, typename AD, typename BD, typename CD, typename DD>
std::vector<T> tetrahedron_monomial_integrals(
  size_t max_dim,
  const Eigen::MatrixBase<AD> &A,
  const Eigen::MatrixBase<BD> &B,
  const Eigen::MatrixBase<CD> &C,
  const Eigen::MatrixBase<DD> &D) {
    // max dim + 1 dimensiosn will be used
    std::vector<T> ret;
    // gotta check why 5 was put here
    size_t max_integral_deg = 2 * max_dim + 2;
    algebra::PascalTriangle pt(max_integral_deg);
    ret.reserve(((max_integral_deg + 1) * (max_integral_deg + 1) * (max_integral_deg + 3)) / 6);
    ;
    auto U = (B - A).eval();
    auto V = (C - A).eval();
    auto W = (D - A).eval();
    T det = U.dot(V.cross(W));

    //std::cout << "U: " << U.transpose() << std::endl;
    //std::cout << "V: " << V.transpose() << std::endl;
    //std::cout << "W: " << W.transpose() << std::endl;

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
                                                    T wterm = W.array().pow(w_exps.template cast<T>().array()).prod() * get_vec_combination(vrem, w_exps);
                                                    int wsum = w_exps.sum();
                                                    //std::cout << "U^(" << u_exps.transpose() << ") ";
                                                    //std::cout << "V^(" << v_exps.transpose() << ") ";
                                                    //std::cout << "W^(" << w_exps.transpose() << ") ";

                                                    //std::cout << " = " << uterm << " * " << vterm << " * " << wterm;
                                                    //std::cout << " = " << uterm * vterm * wterm << std::endl;
                                                    //T f0 = T(1.0) / (usum + 1) * (T(1.0) / (vsum + 1) - T(1.0) / (wsum + vsum + 2));
                                                    //T f1 = T(1.0) / ((vsum + 1) * (usum + vsum + 2));
                                                    //T f2 = T(1.0) / ((usum + vsum + wsum + 3) * (vsum * wsum + 2));

                                                    //spdlog::info("J/K limits: {} {}; denoms min {} {}", wsum + 1, vsum + wsum + 2, vsum + wsum + 2, vsum + wsum + usum + 3);
                                                    T base = uterm * vterm * wterm / (wsum + 1);
                                                    for (int j = 0; j <= wsum + 1; ++j) {

                                                        T jterm = T(pt(wsum + 1, j)) / (vsum + j + 1);
                                                        for (int k = 0; k <= vsum + wsum + 2; ++k) {
                                                            T kterm = T(pt(vsum + wsum + 2, k)) / (wsum + vsum + usum + 3 - j - k);
                                                            kterm = T(pt(vsum + wsum + 2, k)) / (vsum + wsum + usum + 3 - k);


                                                            T val = kterm * jterm * base;

                                                            //spdlog::info("{} =(a^{} b^{} c^{} outer int_[0,1] int_[0,1-a](1-a)^{}b^{} inner a^{}) u{} v{} w{} j{} k{} s{}", val, usum, vsum, wsum, wsum + 1 - j, j, k, uterm, vterm, wterm, jterm, kterm, (j + k % 2 == 0 ? 1 : -1));
                                                            value += val * ((j + k + vsum + wsum) % 2 == 0 ? 1 : -1);
                                                        }
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
                //std::cout << std::endl;
                ret.emplace_back(value * det);
            }
        }
    }
    //std::cout << "Value: " << val << std::endl;
    return ret;
}
}// namespace mtao::geometry
