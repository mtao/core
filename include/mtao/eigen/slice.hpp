#pragma once
#include <Eigen/Dense>
#include "mtao/eigen/concepts.hpp"
#include "mtao/type_utils.hpp"
#include "mtao/iterator/enumerate.hpp"
#include "mtao/eigen/container_size.hpp"
namespace mtao::eigen {


template<concepts::MatrixBaseDerived Mat>
auto slice(const Mat &M, const auto &row_indices, const auto &col_indices) {

    // TODO: is there a better way to do this?
#if defined(EIGEN_SYMBOLIC_INDEX_H)
    return M(row_indices, col_indices);
#else

    using Scalar = typename Mat::Scalar;

    //constexpr static int MatRowsAtCompileTime = Mat::RowsAtCompileTime;
    //constexpr static int MatColsAtCompileTime = Mat::ColsAtCompileTime;

    using RowContainerType = std::decay_t<decltype(row_indices)>;
    using ColContainerType = std::decay_t<decltype(col_indices)>;
    constexpr static int RowsAtCompileTime = container_size<RowContainerType>();
    constexpr static int ColsAtCompileTime = container_size<ColContainerType>();

    constexpr static int Options = Mat::IsRowMajor ? Eigen::RowMajor : Eigen::ColMajor;

    using RetType = Eigen::Matrix<Scalar, RowsAtCompileTime, ColsAtCompileTime, Options>;

    RetType R(row_indices.size(), col_indices.size());
    //Eigen::Matrix<typename Mat::
    if (Mat::IsRowMajor) {
        for (auto &&[ci, cc] : iterator::enumerate(col_indices)) {
            for (auto &&[ri, rc] : iterator::enumerate(row_indices)) {
                R(ri, ci) = Mat(rc, cc);
            }
        }
    } else {
        for (auto &&[ri, rc] : iterator::enumerate(row_indices)) {
            for (auto &&[ci, cc] : iterator::enumerate(col_indices)) {
                R(ri, ci) = Mat(rc, cc);
            }
        }
    }

    return R;
#endif
}

template<concepts::MatrixBaseDerived Mat>
auto row_slice(const Mat &M, const auto &indices) {

    // TODO: is there a better way to do this?
#if defined(EIGEN_SYMBOLIC_INDEX_H)
    return M(indices, Eigen::all);
#else

    using Scalar = typename Mat::Scalar;

    using RowContainerType = std::decay_t<decltype(indices)>;
    constexpr static int RowsAtCompileTime = container_size<RowContainerType>();

    using RetType = Eigen::Matrix<Scalar, RowsAtCompileTime, Mat::ColsAtCompileTime>;

    RetType R(indices.size());
    for (auto &&[i, c] : iterator::enumerate(indices)) {
        R.row(i) = Mat.row(c);
    }

    return R;
#endif
}

template<concepts::MatrixBaseDerived Mat>
auto col_slice(const Mat &M, const auto &indices) {

    // TODO: is there a better way to do this?
#if defined(EIGEN_SYMBOLIC_INDEX_H)
    return M(Eigen::all, indices);
#else

    using Scalar = typename Mat::Scalar;

    using ColContainerType = std::decay_t<decltype(indices)>;
    constexpr static int ColsAtCompileTime = container_size<ColContainerType>();

    using RetType = Eigen::Matrix<Scalar, Mat::RowsAtCompileTime, ColsAtCompileTime>;

    RetType R(indices.size());
    for (auto &&[i, c] : iterator::enumerate(indices)) {
        R.col(i) = Mat.col(c);
    }

    return R;
#endif
}
}// namespace mtao::eigen

