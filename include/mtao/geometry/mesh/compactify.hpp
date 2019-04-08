#pragma once
#include <Eigen/Dense>

namespace mtao::geometry::mesh {

    template <typename VDerived, typename CDerived>
        auto compactify(const Eigen::MatrixBase<VDerived>& V, const Eigen::MatrixBase<CDerived>& C) {

            constexpr static int VRows = VDerived::RowsAtCompileTime;
            using Scalar = typename VDerived::Scalar;
            using RetVerts = mtao::ColVectors<Scalar,VRows>;

            constexpr static int CRows = CDerived::RowsAtCompileTime;
            using Index = typename CDerived::Scalar;
            using RetCells = mtao::ColVectors<Index,CRows>;

            std::map<int,std::tuple<RetVerts,RetCells>> ret;

            RetCells mC = C;
            std::map<int,int> reindexer;
            for(int i = 0; i < C.size(); ++i) {
                Index& v = mC(i);
                if(auto it = reindexer.find(v); it == reindexer.end()) {
                    size_t size = reindexer.size();
                    reindexer[v] = size;
                    v = size;
                } else {
                    v = it->second;
                }
            }
            RetVerts mV(V.rows(),reindexer.size());
            for(auto&& [a,b]: reindexer) {
                mV.col(b) = V.col(a);
            }

            return std::make_tuple(mV,mC);
        }
}
