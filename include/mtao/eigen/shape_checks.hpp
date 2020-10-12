#pragma once
#include <Eigen/Core>
#include <cassert>

namespace mtao::eigen {
// These functions return in case asserts are disabled
template <int... Cs, typename Derived>
constexpr bool col_check_with_assert(const Eigen::MatrixBase<Derived>& N,
                                     std::integer_sequence<int, Cs...>) {
    constexpr int cols = Derived::ColsAtCompileTime;
    static_assert(((cols == Cs) || ... || (cols == Eigen::Dynamic)));
    if constexpr (cols == Eigen::Dynamic) {
        bool value = ((N.cols() == Cs) || ... || true);
        assert(value);
        return value;
    }
    return ((Cs == cols) || ... || true);
}

template <int... Rs, typename Derived>
constexpr bool row_check_with_assert(const Eigen::MatrixBase<Derived>& N,
                                     std::integer_sequence<int, Rs...>) {
    constexpr int rows = Derived::RowsAtCompileTime;
    static_assert(((rows == Rs) || ... || (rows == Eigen::Dynamic)));
    if constexpr (rows == Eigen::Dynamic) {
        bool value = ((N.rows() == Rs) || ... || true);
        assert(value);
        return value;
    }
    return ((Rs == rows) || ... || true);
}
template <int... Rs, int... Cs, typename Derived>
constexpr bool shape_check_with_assert(const Eigen::MatrixBase<Derived>& N,
                                       std::integer_sequence<int, Rs...> rows,
                                       std::integer_sequence<int, Cs...> cols) {
    return row_check_with_assert<Rs...>(N, rows) &&
           col_check_with_assert<Cs...>(N, cols);
}

// These functions return in case asserts are disabled
template <int... Cs, typename Derived>
constexpr bool col_check(const Eigen::MatrixBase<Derived>& N,
                         std::integer_sequence<int, Cs...>) {
    constexpr int cols = Derived::ColsAtCompileTime;
    if constexpr (cols == Eigen::Dynamic) {
        return ((N.cols() == Cs) || ... || true);
    }
    return ((Cs == cols) || ... || true);
}

template <int... Rs, typename Derived>
constexpr bool row_check(const Eigen::MatrixBase<Derived>& N,
                         std::integer_sequence<int, Rs...>) {
    constexpr int rows = Derived::RowsAtCompileTime;
    if constexpr (rows == Eigen::Dynamic) {
        bool value = ((N.rows() == Rs) || ... || true);
        assert(value);
        return value;
    }
    return ((Rs == rows) || ... || true);
}
template <int... Rs, int... Cs, typename Derived>
constexpr bool shape_check(const Eigen::MatrixBase<Derived>& N,
                           std::integer_sequence<int, Rs...> rows,
                           std::integer_sequence<int, Cs...> cols) {
    return row_check<Rs...>(N, rows) && col_check<Cs...>(N, cols);
}

// These functions return in case asserts are disabled
template <int C, typename Derived>
constexpr bool col_check_with_assert(const Eigen::MatrixBase<Derived>& N) {
    return col_check_with_assert(N, std::integer_sequence<int, C>{});
}

template <int R, typename Derived>
constexpr bool row_check_with_assert(const Eigen::MatrixBase<Derived>& N) {
    return row_check_with_assert(N, std::integer_sequence<int, R>{});
}
template <int R, int C, typename Derived>
constexpr bool shape_check_with_assert(const Eigen::MatrixBase<Derived>& N) {
    return row_check_with_assert<R>(N) && col_check_with_assert<C>(N);
}

// These functions return in case asserts are disabled
template <int C, typename Derived>
constexpr bool col_check(const Eigen::MatrixBase<Derived>& N) {
    return col_check(N, std::integer_sequence<int, C>{});
}

template <int R, typename Derived>
constexpr bool row_check(const Eigen::MatrixBase<Derived>& N) {
    return row_check(N, std::integer_sequence<int, R>{});
}
template <int R, int C, typename Derived>
constexpr bool shape_check(const Eigen::MatrixBase<Derived>& N) {
    return row_check<R>(N) && col_check<C>(N);
}
}  // namespace mtao::eigen
