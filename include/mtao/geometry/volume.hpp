#ifndef VOLUME_H
#define VOLUME_H
#include <cassert>
#include <numeric>
#include <stdexcept>

#include "mtao/types.hpp"
#include "mtao/util.h"

namespace mtao {
namespace geometry {

    template<typename Derived>
    auto volume_signed(const Eigen::MatrixBase<Derived> &V) {
        assert(V.cols() == V.rows() + 1);
        auto m = (V.rightCols(V.cols() - 1).colwise() - V.col(0)).eval();
        return m.determinant() / mtao::factorial(m.cols());
    }

    template<typename Derived>
    auto volume_unsigned(const Eigen::MatrixBase<Derived> &V) {
        auto m = (V.rightCols(V.cols() - 1).colwise() - V.col(0)).eval();
        int quotient = mtao::factorial(m.cols());
        return std::sqrt((m.transpose() * m).determinant()) / quotient;
    }

    template<typename Derived>
    auto volume(const Eigen::MatrixBase<Derived> &V) -> typename Derived::Scalar {
        if (V.rows() + 1 == V.cols()) {
            return volume_signed(V);
        } else {
            return volume_unsigned(V);
        }
    }

    template<typename VertexDerived, typename SimplexDerived>
    auto volumes(const Eigen::MatrixBase<VertexDerived> &V,
                 const Eigen::MatrixBase<SimplexDerived> &S) {
        constexpr static int E = VertexDerived::RowsAtCompileTime;// embed dim
        // constexpr static int N = SimplexDerived::ColsAtCompileTime;//number of
        // elements
        constexpr static int D = SimplexDerived::RowsAtCompileTime;// simplex dim
        using Scalar = typename VertexDerived::Scalar;

        mtao::VectorX<Scalar> C(S.cols());

        mtao::Matrix<Scalar, E, D> v(V.rows(), S.rows());
#ifdef _OPENMP
#pragma omp parallel for private(v)
#endif
        for (int i = 0; i < S.cols(); ++i) {
            auto s = S.col(i);
            for (int j = 0; j < s.rows(); ++j) {
                v.col(j) = V.col(s(j));
            }
            C(i) = volume(v);
        }
        return C;
    }

    template<typename VertexDerived, typename SimplexDerived>
    auto brep_volume(const Eigen::MatrixBase<VertexDerived> &V,
                     const Eigen::MatrixBase<SimplexDerived> &S) {
        constexpr static int E = VertexDerived::RowsAtCompileTime;// embed dim
        // constexpr static int N = SimplexDerived::ColsAtCompileTime;//number of
        // elements
        constexpr static int D = SimplexDerived::RowsAtCompileTime;// simplex dim
        using Scalar = typename VertexDerived::Scalar;

        mtao::Matrix<Scalar, E, (D != Eigen::Dynamic) ? (D + 1) : Eigen::Dynamic> v(
          V.rows(), S.rows() + 1);
        v.col(D).setZero();
        Scalar myvol = 0;
        for (int i = 0; i < S.cols(); ++i) {
            auto s = S.col(i);
            for (int j = 0; j < s.rows(); ++j) {
                v.col(j) = V.col(s(j));
            }
            myvol += volume(v);
        }
        return myvol;
    }

    template<typename VertexDerived, typename SimplexDerived>
    auto dual_volumes(const Eigen::MatrixBase<VertexDerived> &V,
                      const Eigen::MatrixBase<SimplexDerived> &S) {
        auto PV = volumes(V, S);
        int elementsPerCell = S.rows();
        using Scalar = typename VertexDerived::Scalar;
        mtao::VectorX<Scalar> Vo = mtao::VectorX<Scalar>::Zero(V.cols());
        for (int i = 0; i < S.cols(); ++i) {
            auto s = S.col(i);
            auto v = PV(i);
            for (int j = 0; j < S.rows(); ++j) {
                Vo(s(j)) += v;
            }
        }
        Vo /= elementsPerCell;
        return Vo;
    }

    template<int D, int S>
    struct dim_specific {};
    template<int S>
    struct dim_specific<2, S> {
        template<typename Derived>
        static auto convex_volume(const Eigen::MatrixBase<Derived> &V) ->
          typename Derived::Scalar {
            using Scalar = typename Derived::Scalar;
            mtao::Matrix<Scalar, Derived::RowsAtCompileTime, S + 1> M(V.rows(),
                                                                      S + 1);

            if constexpr (S == 0) {
                return 1;
            } else {
                if (V.cols() < S + 1) {
                    return 0;
                } else {
                    Scalar r = 0;
                    M.col(0) = V.col(0);

                    for (int j = 1; j < V.cols() - S + 1; ++j) {
                        for (int k = 0; k < S; ++k) {
                            M.col(k + 1) = V.col(j + k);
                        }
                        r += mtao::geometry::volume_unsigned(M);
                    }
                    return r;
                }
            }
        }
    };
    template<typename Derived>
    auto curve_volume(const Eigen::MatrixBase<Derived> &V) ->
      typename Derived::Scalar {
        typename Derived::Scalar ret = 0;
        static_assert(Derived::RowsAtCompileTime == 2 || Derived::RowsAtCompileTime == Eigen::Dynamic);
        if (V.rows() != 2) {
            throw std::invalid_argument("curve_volume only works on ColVecs2d-like inputs");
        }
        for (int i = 0; i < V.cols(); ++i) {
            auto a = V.col(i);
            auto b = V.col((i + 1) % V.cols());
            ret += a.x() * b.y() - a.y() * b.x();
        }
        return .5 * ret;
    }
    template<typename Derived, typename BeginIt, typename EndIt>
    auto curve_volume(const Eigen::MatrixBase<Derived> &V, const BeginIt &beginit, const EndIt &endit) -> typename Derived::Scalar {
        auto it = beginit;
        auto it1 = beginit;
        it1++;
        typename Derived::Scalar ret = 0;
        static_assert(Derived::RowsAtCompileTime == 2 || Derived::RowsAtCompileTime == Eigen::Dynamic);
        if (V.rows() == 2) {
        }
        for (; it != endit; ++it, ++it1) {
            if (it1 == endit) {
                it1 = beginit;
            }
            auto a = V.col(*it);
            auto b = V.col(*it1);
            ret += a.x() * b.y() - a.y() * b.x();
        }
        return .5 * ret;
    }
    template<typename Derived, typename Container>
    auto curve_volume(const Eigen::MatrixBase<Derived> &V, const Container &C) ->
      typename Derived::Scalar {
        return curve_volume(V, C.begin(), C.end());
    }

    template<typename ADerived, typename BDerived, typename CDerived>
    typename ADerived::Scalar triangle_area(const Eigen::MatrixBase<ADerived> &a,
                                            const Eigen::MatrixBase<BDerived> &b,
                                            const Eigen::MatrixBase<CDerived> &c) {
        if constexpr (ADerived::RowsAtCompileTime == 2) {
            return .5 * (b - a).homogeneous().cross((c - a).homogeneous()).z();
        } else if constexpr (ADerived::RowsAtCompileTime == 3) {
            return .5 * (b - a).cross(c - a).norm();
        } else if constexpr (ADerived::RowsAtCompileTime == Eigen::Dynamic) {
            if (a.rows() == 2) {
                return .5 * (b - a).homogeneous().cross((c - a).homogeneous()).z();
            } else if (a.rows() == 3) {
                return .5 * (b - a).cross(c - a).norm();
            }
        }
        // fall through is to use heron's formula
        auto l0 = (b - a).norm();
        auto l1 = (b - c).norm();
        auto l2 = (c - a).norm();
        // auto s = (l0 + l1 + l2) / 2;
        return std::sqrt((l0 + l1 + l2) * (2 * l0 + 2 * l1 - l2) * (2 * l0 - l1 + 2 * l2) * (-l0 + 2 * l1 + 2 * l2)) / 4;
    }

    template<typename ADerived, typename BDerived, typename CDerived, typename DDerived>
    typename ADerived::Scalar tetrahedron_volume(
      const Eigen::MatrixBase<ADerived> &a,
      const Eigen::MatrixBase<BDerived> &b,
      const Eigen::MatrixBase<CDerived> &c,
      const Eigen::MatrixBase<DDerived> &d) {
        return (b - a).cross(c - a).dot(d - a) / 6.;
    }

    // TODO: Check the orientation of these
    template<typename ADerived, typename BDerived>
    typename ADerived::Scalar triangle_area(const Eigen::MatrixBase<ADerived> &a,
                                            const Eigen::MatrixBase<BDerived> &b) {
        return triangle_area(ADerived::Zero(a.size()), a, b);
    }
    template<typename ADerived, typename BDerived, typename CDerived>
    typename ADerived::Scalar tetrahedron_volume(
      const Eigen::MatrixBase<ADerived> &a,
      const Eigen::MatrixBase<BDerived> &b,
      const Eigen::MatrixBase<CDerived> &c) {
        return tetrahedron_volume(ADerived::Zero(a.size()), a, b, c);
    }
}// namespace geometry
}// namespace mtao
#endif// VOLUME_H
