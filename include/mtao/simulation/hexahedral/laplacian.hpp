#pragma once

#include <Eigen/Sparse>
#include <vector>

#include "mtao/eigen/shape_checks.hpp"
#include "mtao/quadrature/simpsons.hpp"

//#include "mtao/geometry/mesh/dual_volumes.hpp"

namespace mtao::simulation::hexahedral {
template <typename Scalar, int D>
Eigen::Matrix<Scalar, 1 << D, 1 << D> laplacian_stencil() {
    using Vec = mtao::Vector<Scalar, D>;
    Eigen::Matrix<Scalar, 1 << D, 1 << D> S;
    S.setZero();
    auto eval = [](int elem, const std::array<Scalar, D>& vec) {
        Vec coeffs;
        Vec grad;
        for (size_t idx = 0; idx < D; ++idx) {
            if (elem & (1 << idx)) {
                grad(idx) = -1;
                coeffs(idx) = (Scalar(1) - vec[idx]);
            } else {
                grad(idx) = 1;
                coeffs(idx) = (vec[idx]);
            }
        }
        for (size_t idx = 0; idx < D; ++idx) {
            for (size_t j = 0; j < D; ++j) {
                if (j != idx) {
                    grad(idx) *= coeffs(j);
                }
            }
        }

        return grad;
    };

    auto quad = [&](int i, int j) {
        const int num_samples = 256;
        Scalar val = Scalar(0);

        val = quadrature::multidim_simpsons_rule<D, Scalar>(
            [&](const std::array<Scalar, D>& p) -> Scalar {
                return eval(i, p).dot(eval(j, p));
            },
            0., 1., num_samples);
        return val;
    };

    /*
    std::array<Scalar, D + 1> memo;
    for (int j = 0; j <= D; ++j) {
        memo[j] = quad(0, (1 << (j)) - 1);
        std::cout << memo[j] << std::endl;
    }
    */
    for (int i = 0; i < S.rows(); ++i) {
        S(i, i) = quad(i, i);
        // S(i, i) = memo[0];
        for (int j = i + 1; j < S.cols(); ++j) {
            /*
            int v = i ^ j;
            int count = 0;
            for (int u = 0; u < D; ++u) {
                if (v & (1 << u)) {
                    count++;
                }
            }
            */
            // S(j, i) = S(i, j) = memo[count];
            S(j, i) = S(i, j) = quad(i, j);
        }
    }
    return S;
}

}  // namespace mtao::simulation::hexahedral
