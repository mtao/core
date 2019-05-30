#pragma once

#include <mtao/types.h>
#include <Eigen/Sparse>
#include <vector>
#include <set>
#include <mtao/geometry/mesh/cell_indexer.h>


//Takes a mesh C and its faces F and creates a matrix where each entry (i,j) points to the face comprised of every vertex except hte one at that index. i.e C(i,j) \cap F(R(i,j)) = C(j)
//R is the returned value
namespace mtao { namespace geometry { namespace mesh {

    namespace internal {
    template <bool Primal,typename Derived>
        std::vector<std::set<int>> boundary_elements_impl(const Eigen::SparseCompressedBase<Derived>& B) {
            using II = typename Derived::InnerIterator;
            std::vector<std::set<int>> ret(Primal?B.cols():B.rows());
            for(int o = 0; o < B.outerSize(); ++o) {
                for(II it(B,o); it; ++it) {
                    if constexpr(Primal) {
                    ret[it.col()].insert(it.row());
                    } else {
                        ret[it.row()].insert(it.col());
                    }
                }
            }
            return ret;
        }
    }

    template <typename Derived>
        std::vector<std::set<int>> boundary_elements(const Eigen::SparseCompressedBase<Derived>& B) {
            return internal::boundary_elements_impl<true>(B);
        }

    template <typename Derived>
        std::vector<std::set<int>> coboundary_elements(const Eigen::SparseCompressedBase<Derived>& B) {
            return internal::boundary_elements_impl<false>(B);
        }


    template <int CellSize, typename CellType, typename FacetType>
        auto boundary_elements_sized(const Eigen::MatrixBase<CellType>& C, const Eigen::MatrixBase<FacetType>& F) {


            constexpr static int FacetSize = CellSize - 1;

            static_assert((FacetSize == FacetType::RowsAtCompileTime) || (FacetType::RowsAtCompileTime == Eigen::Dynamic));
            static_assert(CellSize == FacetSize+1);

            using Index = typename CellType::Scalar;

            using FacetArray = typename std::array<Index,FacetSize>;

            using FacetVector = typename mtao::Vector<Index,FacetSize>;

            CellIndexer<FacetSize> fi(F);

            using RetType = Eigen::Matrix<Index, CellType::RowsAtCompileTime, CellType::ColsAtCompileTime>;


            RetType R = CellType::Constant(C.rows(),C.cols(),-1);

            FacetArray f;
            Eigen::Map<FacetVector> fm(f.data());
            FacetArray f2;
            Eigen::Map<FacetVector> fm2(f2.data());
            for(int i = 0; i < C.cols(); ++i) {
                auto c = C.col(i);
                Index cs = c.sum();

                std::map<int,int> cm;
                for(int i = 0; i < c.rows(); ++i) {
                    cm[c(i)] = i;
                }
                auto r = R.col(i);

                fm = c.template topRows<FacetSize>();
                for(int j = 0; j < CellSize; ++j) {
                    fm2 = fm;
                    int fidx = fi(f2);
                    auto f = F.col(fidx);


                    r(cm[cs - f.sum()]) = fidx;
                    

                    auto unused = (j + FacetSize)%CellSize;
                    fm(j%FacetSize) = c(unused);

                }
            }

            return R;
        }

    template <typename CellType, typename FacetType>
        CellType boundary_elements(const Eigen::MatrixBase<CellType>& C, const Eigen::MatrixBase<FacetType>& F) {

            constexpr static int CellSize = CellType::RowsAtCompileTime;
            constexpr static int FacetSize = FacetType::RowsAtCompileTime;

            static_assert(CellSize == FacetSize+1);

            return boundary_elements_sized<CellSize>(C,F);
        }


}}}

