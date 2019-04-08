
#pragma once

namespace mtao::geometry::mesh {

    /*
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
    */
    template <typename VDerived, typename CDerived>
        auto separate_elements(const Eigen::MatrixBase<VDerived>& V, const std::map<int,CDerived>& Cs) {

            constexpr static int VRows = VDerived::RowsAtCompileTime;
            using Scalar = typename VDerived::Scalar;
            using RetVerts = mtao::ColVectors<Scalar,VRows>;

            constexpr static int CRows = CDerived::RowsAtCompileTime;
            using Index = typename CDerived::Scalar;
            using RetCells = mtao::ColVectors<Index,CRows>;

            std::map<int,std::tuple<RetVerts,RetCells>> ret;

            for(auto&& [idx,C]: Cs) {
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

                ret[idx] = std::make_tuple(mV,mC);
            }
            return ret;
        }

    template <typename VDerived, typename CDerived>
        auto unify(const std::map<int,std::tuple<VDerived,CDerived>>& VM) {
            constexpr static int VRows = VDerived::RowsAtCompileTime;
            using Scalar = typename VDerived::Scalar;
            using RetVerts = mtao::ColVectors<Scalar,VRows>;

            constexpr static int CRows = CDerived::RowsAtCompileTime;
            using Index = typename CDerived::Scalar;
            using RetCells = mtao::ColVectors<Index,CRows>;
            int vcols = 0;
            int ccols = 0;
            int vrows = 0;
            int crows = 0;

            for(auto&& [i,VC]: VM) {
                auto [V,C] = VC;
                vcols += V.cols();
                ccols += C.cols();
                vrows = std::max<int>(vrows,V.rows());
                crows = std::max<int>(crows,C.rows());
            }
            RetVerts mV(vrows,vcols);
            RetCells mC(crows,ccols);
            std::vector<int> blocks;
            vcols = 0;
            ccols = 0;
            for(auto&& [i,VC]: VM) {
                auto&& [V,C] = VC;
                mV.block(0,vcols,V.rows(),V.cols()) = V;
                mC.block(0,ccols,C.rows(),C.cols()) = C.array() + vcols;
                blocks.push_back(vcols);
                vcols += V.cols();
                ccols += C.cols();
            }
            blocks.push_back(vcols);
            return std::tuple<RetVerts,RetCells,std::vector<int>>(mV,mC,blocks);
        }
    template <typename VDerived, typename CDerived>
        auto separate_elements_unified(const Eigen::MatrixBase<VDerived>& V, const std::map<int,CDerived>& Cs) {
            return unify(separate_elements(V,Cs));
        }
}
