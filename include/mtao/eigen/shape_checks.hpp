#pragma once
#include <Eigen/Core>
#include "mtao/eigen/concepts.hpp"
#include <fmt/format.h>
#include <fmt/ranges.h>

namespace mtao::eigen {

namespace concepts {
    template<int R, typename T>
    concept RowCompatible = DenseBaseDerived<T> && ((T::RowsAtCompileTime == R) || (T::RowsAtCompileTime == Eigen::Dynamic));
    template<int C, typename T>
    concept ColCompatible = DenseBaseDerived<T> && ((T::ColsAtCompileTime == C) || (T::ColsAtCompileTime == Eigen::Dynamic));
    template<int R, int C, typename T>
    concept ShapeCompatible = DenseBaseDerived<T> &&RowCompatible<R, T> &&ColCompatible<C, T>;


    template<typename T>
    concept VecCompatible = MatrixBaseDerived<T> &&ColCompatible<1, T>;
    template<typename T>
    concept RowVecCompatible = MatrixBaseDerived<T> &&RowCompatible<1, T>;


    template<int D, typename T>
    concept SquareMatrixDCompatible = MatrixBaseDerived<T> &&ShapeCompatible<D, D, T>;

    template<typename T>
    concept SquareMatrix2Compatible = MatrixBaseDerived<T> &&SquareMatrixDCompatible<2, T>;
    template<typename T>
    concept SquareMatrix3Compatible = MatrixBaseDerived<T> &&SquareMatrixDCompatible<3, T>;
    template<typename T>
    concept SquareMatrix4Compatible = MatrixBaseDerived<T> &&SquareMatrixDCompatible<4, T>;

    template<typename T>
    concept ColVecs2Compatible = MatrixBaseDerived<T> &&RowCompatible<2, T>;
    template<typename T>
    concept ColVecs3Compatible = MatrixBaseDerived<T> &&RowCompatible<3, T>;
    template<typename T>
    concept ColVecs4Compatible = MatrixBaseDerived<T> &&RowCompatible<4, T>;


    // TODO: will i someday figureo ut how to template this?
    template<typename T>
    concept RowVecs2Compatible = MatrixBaseDerived<T> &&ColCompatible<2, T>;
    template<typename T>
    concept RowVecs3Compatible = MatrixBaseDerived<T> &&ColCompatible<3, T>;
    template<typename T>
    concept RowVecs4Compatible = MatrixBaseDerived<T> &&ColCompatible<4, T>;


    template<typename T>
    concept VecCompatible = MatrixBaseDerived<T> &&RowCompatible<1, T>;

    template<typename T>
    concept RowVecCompatible = ColCompatible<1, T>;

    template<int R, typename T>
    concept VecDCompatible = MatrixBaseDerived<T> &&ShapeCompatible<R, 1, T>;


    template<typename T>
    concept Vec2Compatible = VecDCompatible<2, T>;
    template<typename T>
    concept Vec3Compatible = VecDCompatible<3, T>;
    template<typename T>
    concept Vec4Compatible = VecDCompatible<4, T>;


}// namespace concepts
// These functions return in case asserts are disabled
template<int... Cs, typename MatType>
requires(concepts::ColCompatible<Cs, MatType> || ... || false) constexpr bool col_check_oneof(const MatType &N,
                                                                                              std::integer_sequence<int, Cs...>) {

    constexpr int cols = MatType::ColsAtCompileTime;
    if constexpr (cols == Eigen::Dynamic) {
        return ((N.cols() == Cs) || ... || false);
    }
    return true;
}

template<int... Rs, typename MatType>
requires(concepts::RowCompatible<Rs, MatType> || ... || false) constexpr bool row_check_oneof(const MatType &N,
                                                                                              std::integer_sequence<int, Rs...>) {
    constexpr int rows = MatType::RowsAtCompileTime;
    if constexpr (rows == Eigen::Dynamic) {
        return ((N.rows() == Rs) || ... || false);
    }
    return true;
}

template<int C, typename MatType>
requires concepts::ColCompatible<C, MatType> constexpr bool col_check(const MatType &N) {
    return col_check_oneof(N, std::integer_sequence<int, C>{});
}

template<int R, typename MatType>
requires concepts::RowCompatible<R, MatType> constexpr bool row_check(const MatType &N) {
    return row_check_oneof(N, std::integer_sequence<int, R>{});
}
template<int R, int C, typename MatType>
constexpr bool shape_check(const MatType &N) {
    return row_check<R>(N) && col_check<C>(N);
}

template<int C, typename MatType>
requires concepts::ColCompatible<C, MatType> constexpr void col_check_with_throw(const MatType &N) {
    if (!col_check<C>(N)) {
        throw std::invalid_argument(fmt::format("Col check: wrong size, got {} expected {}", N.cols(), C));
    }
}

template<int R, typename MatType>
requires concepts::RowCompatible<R, MatType> constexpr void row_check_with_throw(const MatType &N) {

    if (!row_check<R>(N)) {
        throw std::invalid_argument(fmt::format("Row check: wrong size, got {} expected {}", N.rows(), R));
    }
}
template<int R, int C, typename MatType>
constexpr void shape_check_with_throw(const MatType &N) {
    row_check_with_throw<R>(N);
    col_check_with_throw<C>(N);
}

template<int... C, typename MatType>
constexpr void col_check_with_throw(const MatType &N, std::integer_sequence<int, C...> Seq) {
    if (!col_check_oneof(N, Seq)) {
        std::string msg = fmt::format("Col check: wrong size, got {} expected one of {}", N.cols(), std::make_tuple(C...));
        throw std::invalid_argument(msg);
    }
}

template<int... R, typename MatType>
constexpr void row_check_with_throw(const MatType &N, std::integer_sequence<int, R...> Seq) {

    if (!row_check_oneof(N, Seq)) {
        std::string msg = fmt::format("Row check: wrong size, got {} expected {}", N.rows(), std::make_tuple(R...));
        throw std::invalid_argument(msg);
    }
}
template<int... R, int... C, typename MatType>
constexpr void shape_check_with_throw(const MatType &N, std::integer_sequence<int, R...> RSeq, std::integer_sequence<int, C...> CSeq) {
    row_check_with_throw(N, RSeq) && col_check_with_throw(N, CSeq);
}
}// namespace mtao::eigen
