
#pragma once

namespace mtao::geometry::mesh {
    template <typename VDerived, typename CDerived>
        auto separate_colvecs(const Eigen::MatrixBase<VDerived>& V, const Eigen::MatrixBase<CDerived>& C) {

            constexpr static int VRows = VDerived::RowsAtCompileTime;
            using Scalar = typename VDerived::Scalar;
            using RetVerts = mtao::ColVectors<Scalar,VRows>;

            RetVerts RV(V.rows(),C.size());
            int index = 0;
            for(int i = 0; i < C.rows(); ++i) {
                auto c = C.col(i);
                for(int j = 0; j < C.cols(); ++j) {
                    RV.col(index++) = V.col(c(j));
                }
            }
            return RV;

        }
    template <typename CDerived>
        auto separate_cells(const Eigen::MatrixBase<CDerived>& C) {

            constexpr static int CRows = CDerived::RowsAtCompileTime;
            using Index = typename CDerived::Scalar;
            using RetCells = mtao::ColVectors<Index,CRows>;

            RetCells RC(C.rows(),C.cols());
            int index = 0;
            for(int i = 0; i < C.rows(); ++i) {
                auto c = C.col(i);
                auto rc = RC.col(i);
                for(int j = 0; j < C.cols(); ++j) {
                    rc(j) = index++;
                }
            }
            return RV;

        }
}
