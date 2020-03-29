#pragma once
#include <Eigen/Core>
#include <cassert>

namespace mtao::eigen {
// These functions return in case asserts are disabled
template <int C, typename Derived>
constexpr bool col_check(const Eigen::MatrixBase<Derived>& N) {
    constexpr int cols = Derived::ColsAtCompileTime;
    static_assert(cols == C || cols == Eigen::Dynamic);
    if constexpr (cols == Eigen::Dynamic) {
        assert(N.cols() == C);
        return N.cols() == C;
    }
    return true;
}

template <int R, typename Derived>
constexpr bool row_check(const Eigen::MatrixBase<Derived>& N) {
    constexpr int rows = Derived::RowsAtCompileTime;
    static_assert(rows == R || rows == Eigen::Dynamic);
    if constexpr (rows == Eigen::Dynamic) {
        assert(N.rows() == R);
        return N.rows() == R;
    }
    return true;
}
template <int R, int C, typename Derived>
constexpr bool shape_check(const Eigen::MatrixBase<Derived>& N) {
    return row_check<R>(N) && col_check<C>(N);
}

}  // namespace mtao::eigen
