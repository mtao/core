#pragma once
#include <vector>
#include "mass_matrix.hpp"
#include <Eigen/Sparse>
#include "opposite_volumes.hpp"

namespace mtao { namespace geometry { namespace mesh {
    template <typename VertexDerived, typename SimplexDerived> 
        auto cot_laplacian( const Eigen::MatrixBase<VertexDerived>& V, const Eigen::MatrixBase<SimplexDerived>& S) {
            using Scalar = typename VertexDerived::Scalar;
            using Triplet = Eigen::Triplet<double>;
            std::vector<Triplet> trips;
            int mV = V.cols();
            Eigen::SparseMatrix<Scalar> L(mV,mV);
            assert(S.rows() == 3);
            auto OV = opposite_volumes(V,S);
            std::vector<double> diag(mV,0);
            for(int fi = 0; fi < S.cols(); ++fi) {
                auto&& le = OV.col(fi).array();
                auto&& f = S.col(fi);

                double s = (le.sum()) / 2.0;
                double area = std::sqrt(s * (s-le).prod());
                double d = le.prod() / (2 * area);

                auto Sa = le/d;

                for(int i = 0; i < 3; ++i) {
                    double a = le((i+0)%3);
                    double b = le((i+1)%3);
                    double c = le((i+2)%3);
                    double ca = ( b*b + c*c - a*a ) / ( 2 * b*c );
                    double v = -.5 * ca / Sa(i);


                    //int A = f((i+0)%3);
                    int B = f((i+1)%3);
                    int C = f((i+2)%3);
                    trips.emplace_back(B,C,v);
                    trips.emplace_back(C,B,v);
                    diag[B] += -v;
                    diag[C] += -v;
                }

            }
            for(int i = 0; i < diag.size(); ++i) {
                trips.emplace_back(i,i,diag[i]);
            }



            L.setFromTriplets(trips.begin(),trips.end());
            return L;
        }

    template <typename VertexDerived, typename SimplexDerived> 
        auto strong_cot_laplacian( const Eigen::MatrixBase<VertexDerived>& V, const Eigen::MatrixBase<SimplexDerived>& S) {
            using Scalar = typename VertexDerived::Scalar;
            auto DM = mass_matrix(V,S).array().eval();
            DM = (DM.abs() > 1e-6).select(1/DM,0);
            Eigen::SparseMatrix<Scalar> L = DM.matrix().asDiagonal() * cot_laplacian(V,S);
            return L;
        }
}
}
}
