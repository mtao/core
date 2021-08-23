#pragma once
#include "mtao/geometry/bishop_frame.hpp"


namespace mtao::geometry {

template<typename T>
std::tuple<mtao::ColVectors<T, 3>, mtao::ColVecs3i> tube(const mtao::ColVectors<T, 3> &P, double radius, int subdivisions = 5) {
    using CVec = mtao::ColVectors<T, 3>;
    using VecX = mtao::VectorX<T>;

    VecX theta = VecX::LinSpaced(subdivisions + 1, 0, 2 * std::numbers::pi_v<T>).head(subdivisions);


    mtao::ColVectors<T, 2> CS = mtao::eigen::hstack(theta.array().cos(), theta.array().sin()).transpose();

    CVec RV(3, subdivisions * P.cols());

    auto [TT, U] = bishop_frame(P);
    CVec V(3, P.cols());
    mtao::ColVecs3i RF(3, 2 * (P.cols() - 1) * subdivisions);
    for (int i = 0; i < V.cols(); ++i) {
        V.col(i) = TT.col(i).cross(U.col(i));
    }


    for (int i = 0; i < P.cols(); ++i) {
        auto p = P.col(i);
        auto u = U.col(i);
        auto v = V.col(i);
        int voff = subdivisions * i;
        auto r = RV(Eigen::all, Eigen::seqN(subdivisions * i, subdivisions));
        r = radius * mtao::eigen::hstack(u, v) * CS;
        r.colwise() += p;

        if (i == P.cols() - 1) continue;
        auto f = RF(Eigen::all, Eigen::seqN(2 * subdivisions * i, 2 * subdivisions));
        for (int j = 0; j < subdivisions; ++j) {
            int u = voff + j;
            int v = voff + (j + 1) % subdivisions;
            int s = voff + subdivisions + j;
            int t = voff + subdivisions + (j + 1) % subdivisions;
            f.col(2 * j) << u, v, s;
            f.col(2 * j + 1) << s, v, t;
        }
    }


    return { RV, RF };
}
//only bottom half capped
template<typename T>
std::tuple<mtao::ColVectors<T, 3>, mtao::ColVecs3i> half_capped_tube(const mtao::ColVectors<T, 3> &P, double radius, int subdivisions = 5) {
    auto [V, F] = tube(P, radius, subdivisions);
    mtao::ColVecs3i CF(3, subdivisions - 2);
    for (int i = 1; i < subdivisions - 1; ++i) {
        CF.col(i - 1) << 0, i, i + 1;
    }
    F = mtao::eigen::hstack(F, CF);

    return { V, F };
}
template<typename T>
std::tuple<mtao::ColVectors<T, 3>, mtao::ColVecs3i> capped_tube(const mtao::ColVectors<T, 3> &P, double radius, int subdivisions = 5) {
    auto [V, F] = tube(P, radius, subdivisions);
    mtao::ColVecs3i CF(3, subdivisions - 2);
    for (int i = 1; i < subdivisions - 1; ++i) {
        CF.col(i - 1) << 0, i + 1, i;
    }
    F = mtao::eigen::hstack(F, CF, CF({ 0, 2, 1 }, Eigen::all).array() + (P.cols() - 1) * subdivisions);

    return { V, F };
}
}// namespace mtao::geometry
