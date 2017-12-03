#ifndef VOLUME_H
#define VOLUME_H
#include <Eigen/Dense>
#include "types.h"
#include "util.h"
#include <numeric>
#include <cassert>

#include "logging/logger.hpp"
#include "logging/timer.hpp"
using namespace mtao::logging;
namespace mtao { namespace geometry {

    template <typename Derived>
        auto volume_signed( const Eigen::MatrixBase<Derived> & V) {
            assert(V.cols() == V.rows() + 1);
            auto m = (V.rightCols(V.cols()-1).colwise() - V.col(0)).eval();
            return m.determinant() / mtao::factorial(m.cols());
        }

    template <typename Derived>
        auto volume_unsigned( const Eigen::MatrixBase<Derived> & V) {
            auto m = (V.rightCols(V.cols()-1).colwise() - V.col(0)).eval();
            int quotient = mtao::factorial(m.cols());
            return std::sqrt((m.transpose() * m).determinant()) / quotient;
        }

    template <typename Derived>
        auto volume( const Eigen::MatrixBase<Derived> & V) -> typename  Derived::Scalar{
            if(V.rows()+1 == V.cols()) {
                return volume_signed(V);
            } else {
                return volume_unsigned(V);
            }
        }

    template <typename VertexDerived, typename SimplexDerived> 
        auto volumes( const Eigen::MatrixBase<VertexDerived>& V, const Eigen::MatrixBase<SimplexDerived>& S) {
            constexpr static int E = VertexDerived::RowsAtCompileTime;//embed dim
            constexpr static int N = SimplexDerived::ColsAtCompileTime;//number of elements
            constexpr static int D = SimplexDerived::RowsAtCompileTime;//simplex dim
            using Scalar = typename VertexDerived::Scalar;

            Eigen::Matrix<Scalar,E,N> C(V.rows(),S.cols());

            Eigen::Matrix<Scalar,E,D> v(V.rows(),S.rows());
            for(int i = 0; i < S.cols(); ++i) {
                auto s = S.col(i);
                for(int j = 0; j < S.rows(); ++j) {
                    v.col(j) = V.col(s(j));
                }
                C.col(i) = volume(v);
            }
            return C;
        }

    template <int D, int S>
        struct dim_specific {
        };
    template <int S>
        struct dim_specific<2, S> {
            template <typename Derived>
                static auto convex_volume( const Eigen::MatrixBase<Derived> & V) -> typename Derived::Scalar {
                    using Scalar = typename Derived::Scalar;
                    mtao::Matrix<Scalar, Derived::RowsAtCompileTime, S+1> M(V.rows(),S+1);

                    if constexpr(S == 0) {
                        return 1;
                    } else {
                        if(V.cols() < S+1) {
                            return 0;
                        } else {
                            Scalar r = 0;
                            M.col(0) = V.col(0);


                            for(int j = 1; j < V.cols()-S+1; ++j) {
                                for(int k = 0; k < S; ++k) {
                                    M.col(k+1) = V.col(j+k);
                                }
                                r += mtao::geometry::volume_unsigned(M);
                            }
                            return r;
                        }
                    }
                }
        };
}}
#endif//VOLUME_H
