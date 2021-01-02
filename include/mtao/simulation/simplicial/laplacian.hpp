#pragma once

#include <Eigen/Sparse>
#include <vector>

#include "mtao/eigen/shape_checks.hpp"
#include "mtao/geometry/volume.hpp"

namespace mtao::simulation::simplicial {
template<typename VDerived, typename SDerived>
Eigen::SparseMatrix<typename VDerived::Scalar> laplacian(
  const Eigen::MatrixBase<VDerived> &V,
  const Eigen::MatrixBase<SDerived> &S);

namespace internal {
    // NOTE: yes we do have a second impl in mtao/geometry/mesh/laplacian.hpp
    template<typename VDerived, typename SDerived>
    Eigen::SparseMatrix<typename VDerived::Scalar> laplacian2(
      const Eigen::MatrixBase<VDerived> &V,
      const Eigen::MatrixBase<SDerived> &S) {
        using Scalar = typename VDerived::Scalar;
        if (S.size() == 0) {
            return {};
        }
        int size = S.maxCoeff() + 1;
        Eigen::SparseMatrix<Scalar> L(size, size);
        std::vector<Eigen::Triplet<Scalar>> trips;
        trips.reserve(4 * 3 * S.cols());
        eigen::row_check_with_assert<3>(S);
        assert(V.cols() >= size);

        for (int sidx = 0; sidx < S.cols(); ++sidx) {
            auto s = S.col(sidx);
            for (int j = 0; j < S.rows(); ++j) {
                const auto ai = s(j);
                const auto bi = s((j + 1) % 3);
                const auto ci = s((j + 2) % 3);
                auto a = V.col(ai);
                auto b = V.col(bi);
                auto c = V.col(ci);
                auto u = b - a;
                auto v = c - a;

                Scalar dot = u.dot(v);
                Scalar cross;
                if constexpr (VDerived::RowsAtCompileTime == 2) {
                    cross = u.x() * v.y() - u.y() * v.x();
                } else if constexpr (VDerived::RowsAtCompileTime == 3) {
                    cross = u.cross(v);
                }
                Scalar val = std::abs(.5 * dot / cross);
                trips.emplace_back(bi, ci, -val);
                trips.emplace_back(ci, bi, -val);
                trips.emplace_back(bi, bi, val);
                trips.emplace_back(ci, ci, val);
            }
        }
        L.setFromTriplets(trips.begin(), trips.end());
        return L;
    }


    template<typename VDerived, typename SDerived>
    Eigen::SparseMatrix<typename VDerived::Scalar> laplacian3(
      const Eigen::MatrixBase<VDerived> &V,
      const Eigen::MatrixBase<SDerived> &S) {
        using Scalar = typename VDerived::Scalar;
        if (S.size() == 0) {
            return {};
        }
        int size = S.maxCoeff() + 1;
        Eigen::SparseMatrix<Scalar> L(size, size);
        std::vector<Eigen::Triplet<Scalar>> trips;
        trips.reserve(4 * 4 * S.cols());

        eigen::row_check_with_assert<4>(S);
        eigen::row_check_with_assert<3>(V);

        auto vols = geometry::volumes(V, S);

        assert(V.cols() >= size);
        for (int sidx = 0; sidx < S.cols(); ++sidx) {
            auto s = S.col(sidx);
            auto vol = vols(sidx);
            mtao::Matrix<Scalar, 3, 4> N;

            for (int j = 0; j < S.rows(); ++j) {
                const auto ai = s(j);
                const auto bi = s((j + 1) % 4);
                const auto ci = s((j + 2) % 4);
                const auto di = s((j + 3) % 4);
                auto a = V.col(ai);
                auto b = V.col(bi);
                auto c = V.col(ci);
                auto d = V.col(di);
                auto u = c - b;
                auto v = d - b;
                auto n = N.col(j);
                n = u.cross(v);
                n.normalize();
                Scalar len = (a - b).dot(n);
                if (len < 0) {
                    n *= -len * len;
                } else {
                    n *= len * len;
                }
            }
            for (int j = 0; j < S.rows(); ++j) {
                const auto ai = s(j);
                auto na = N.col(j);
                for (int k = j + 1; k < S.rows(); ++k) {
                    const auto bi = s(k);
                    auto nb = N.col(k);
                    Scalar val = vol * na.dot(nb);
                    trips.emplace_back(ai, bi, -val);
                    trips.emplace_back(bi, ai, -val);
                    trips.emplace_back(ai, ai, val);
                    trips.emplace_back(bi, bi, val);
                }
            }
        }
        L.setFromTriplets(trips.begin(), trips.end());
        return L;
    }
}// namespace internal

template<typename VDerived, typename SDerived>
Eigen::SparseMatrix<typename VDerived::Scalar> laplacian(
  const Eigen::MatrixBase<VDerived> &V,
  const Eigen::MatrixBase<SDerived> &S) {
    // TODO: make this use V with constant col sizes
    if constexpr (SDerived::RowsAtCompileTime == 3) {
        return internal::laplacian2(V, S);
    } else if constexpr (SDerived::RowsAtCompileTime == 4) {
        return internal::laplacian3(V, S);
    }
    return {};
}

}// namespace mtao::simulation::simplicial
