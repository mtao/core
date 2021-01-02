#pragma once
#include "mtao/eigen/shape_checks.hpp"
#include <iostream>


namespace mtao::geometry {
template<typename Derived>
auto normal_basis(const Eigen::MatrixBase<Derived> &N) {
    using Scalar = typename Derived::Scalar;

    mtao::Matrix<Scalar, 3, 3> uv;
    if (N.norm() < 1e-10) {
        uv = mtao::Mat3d::Identity();
    } else if (eigen::shape_check<3, 1>(N)) {
        auto u = uv.col(0);
        auto v = uv.col(1);
        for (int i = 0; i < 3; ++i) {
            if (N((i + 1) % 3) != 0) {
                u = N.cross(mtao::Vector<Scalar, 3>::Unit(i));
                break;
            }
        }
        v = u.cross(N);
        uv.col(2) = N;
        uv.colwise().normalize();
    } else {
        std::cerr << "Bad input shape?" << std::endl;
    }
    return uv;
}
}// namespace mtao::geometry
