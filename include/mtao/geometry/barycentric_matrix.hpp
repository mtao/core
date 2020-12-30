#pragma once
#include "mtao/types.hpp"
#include <Eigen/Sparse>


namespace mtao::geometry {
//A collection of simplicies, indices into simplices, and related barycentric coordinates
template<typename SimplexType, typename BType, typename SIType>
Eigen::SparseMatrix<typename BType::Scalar>
  barycentric_matrix(int num_vertices, const Eigen::MatrixBase<SimplexType> &S, const Eigen::MatrixBase<SIType> &SI, const Eigen::MatrixBase<BType> &B) {
    assert(SI.size() == B.cols());


    using Scalar = typename BType::Scalar;
    Eigen::SparseMatrix<Scalar> A(B.cols(), num_vertices);

    std::vector<Eigen::Triplet<Scalar>> trips;
    trips.reserve(S.rows() * SI.size());

    for (int i = 0; i < B.cols(); ++i) {
        auto s = S.col(SI(i));
        auto b = B.col(i);
        for (int j = 0; j < b.rows(); ++j) {
            trips.emplace_back(i, s(j), b(j));
        }
    }
    A.setFromTriplets(trips.begin(), trips.end());
    return A;
}
//A collection of simplicies, indices into simplices, and related barycentric coordinates
//piecewise constant per-face values
template<typename Scalar, typename SIType>
Eigen::SparseMatrix<Scalar>
  barycentric_matrix_face(int num_faces, const Eigen::MatrixBase<SIType> &SI) {

    Eigen::SparseMatrix<Scalar> A(SI.rows(), num_faces);

    std::vector<Eigen::Triplet<Scalar>> trips;
    trips.reserve(SI.size());

    for (int i = 0; i < SI.cols(); ++i) {
        trips.emplace_back(i, SI(i), 1);
    }
    A.setFromTriplets(trips.begin(), trips.end());
    return A;
}
}// namespace mtao::geometry
