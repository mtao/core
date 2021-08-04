#pragma once
#include "mtao/geometry/bishop_frame.hpp"
#include "mtao/geometry/mesh/shapes/disc.hpp"
#include <iostream>


namespace mtao::geometry::mesh::shapes {
namespace internal {
    struct TubeConstructor {
        double radius;
        int subdivisions = 5;
        bool back = false;
        bool front = false;
        template<typename T>
        mtao::ColVectors<T, 3> create_vertices(const mtao::ColVectors<T, 3> &P) const;
        template<typename T, int D>
        mtao::ColVectors<T, D> create_per_ring_data(const mtao::ColVectors<T, D> &P) const;
        mtao::ColVecs3i create_triangulation(int segment_count) const;
    };

    template<typename T>
    std::tuple<mtao::ColVectors<T, 3>, mtao::ColVecs3i> tube(const mtao::ColVectors<T, 3> &P, double radius, int subdivisions, bool back, bool front) {
        TubeConstructor tc{ radius, subdivisions, back, front };
        return { tc.create_vertices(P), tc.create_triangulation(P.cols()) };
    }
}// namespace internal


//only back half capped
template<typename T>
std::tuple<mtao::ColVectors<T, 3>, mtao::ColVecs3i> tube(const mtao::ColVectors<T, 3> &P, double radius, int subdivisions = 5, bool back = false, bool front = false) {
    return internal::tube(P, radius, subdivisions, back, front);
}
template<typename T>
std::tuple<mtao::ColVectors<T, 3>, mtao::ColVecs3i> capped_tube(const mtao::ColVectors<T, 3> &P, double radius, int subdivisions = 5) {
    return internal::tube(P, radius, subdivisions, true, true);
}
namespace internal {
    template<typename T, int D>
    mtao::ColVectors<T, D> TubeConstructor::create_per_ring_data(const mtao::ColVectors<T, D> &P) const {
        using CVec = mtao::ColVectors<T, D>;
        CVec RV(D, subdivisions * P.cols());
        for (int i = 0; i < P.cols(); ++i) {
            auto p = P.col(i);
            int voff = subdivisions * i;
            auto r = RV.block(0, voff, RV.rows(), subdivisions);
            r.colwise() = p;
        }
    }
    template<typename T>
    mtao::ColVectors<T, 3> TubeConstructor::create_vertices(const mtao::ColVectors<T, 3> &P) const {

        using VecX = mtao::VectorX<T>;
        VecX theta = VecX::LinSpaced(subdivisions + 1, 0, 2 * M_PI).head(subdivisions);


        mtao::ColVectors<T, 2> CS = mtao::eigen::hstack(theta.array().cos(), theta.array().sin()).transpose();

        using CVec = mtao::ColVectors<T, 3>;
        CVec RV = create_per_ring_data(P);

        auto [TT, U] = bishop_frame(P);
        CVec V(3, P.cols());
        for (int i = 0; i < V.cols(); ++i) {
            V.col(i) = TT.col(i).cross(U.col(i));
        }


        for (int i = 0; i < P.cols(); ++i) {
            auto u = U.col(i);
            auto v = V.col(i);
            int voff = subdivisions * i;
            auto r = RV.block(0, voff, RV.rows(), subdivisions);
            r += radius * mtao::eigen::hstack(u, v) * CS;
        }


        return RV;
    }
}// namespace internal
}// namespace mtao::geometry::mesh::shapes
