#ifndef BOUNDARY_MATRIX_H
#define BOUNDARY_MATRIX_H

#include <mtao/types.hpp>
#include <Eigen/Sparse>

namespace mtao { namespace geometry { namespace mesh {

    template <typename T, typename CellType, typename FacetType>
        auto boundary_matrix(const Eigen::MatrixBase<CellType>& C, const Eigen::MatrixBase<FacetType>& F) -> Eigen::SparseMatrix<T> {

            constexpr static int CellSize = CellType::RowsAtCompileTime;
            constexpr static int FacetSize = FacetType::RowsAtCompileTime;

            using Index = typename CellType::Scalar;

            using FacetArray = typename std::array<Index,FacetSize>;
            //using CellArray = typename std::array<Index,CellSize>;

            using FacetVector = typename mtao::Vector<Index,FacetSize>;
            //using CellVector = typename mtao::Vector<Index,CellSize>;

            std::map<std::array<Index,FacetSize>,std::tuple<int,T>> facet_map;

            auto swap_and_sign = [](auto&& f) {
                T sign = 1;
                for(unsigned int i = 0; i < f.size(); ++i) {
                    auto&& vi = f[i];
                    for(unsigned int j = i+1; j < f.size(); ++j) {
                        auto&& vj = f[j];
                        if(vj < vi) {
                            sign *= -1;
                            std::swap(vi,vj);
                        }
                    }
                }
                return sign;
            };


            FacetArray f;
            Eigen::Map<FacetVector> fm(f.data());
            for(int i = 0; i < F.cols(); ++i) {
                fm = F.col(i);
                T sign = swap_and_sign(f);
                facet_map.emplace(f,std::tuple<int,T>{i,sign});
            }

            std::vector<Eigen::Triplet<T>> trips;

            FacetArray f2;
            Eigen::Map<FacetVector> fm2(f2.data());
            for(int i = 0; i < C.cols(); ++i) {
                auto c = C.col(i);
                fm = c.template topRows<FacetSize>();
                bool even_perm = false;
                for(int j = 0; j < CellSize; ++j) {
                    fm2 = fm;
                    T sign = swap_and_sign(fm2);
                    if(auto it = facet_map.find(f2); it != facet_map.end()) {
                        auto [idx,fsign] = it->second;
                        trips.emplace_back(idx,i,sign * fsign * (even_perm?1:-1));
                    } else {
                        //TODO: This should never happen
                    }
                    auto unused = (j + FacetSize)%CellSize;
                    fm(j%FacetSize) = c(unused);
                    even_perm ^= true;

                }
            }
            Eigen::SparseMatrix<T> A(F.cols(), C.cols());
            A.setFromTriplets(trips.begin(),trips.end());

            return A;
        }

    //Edges -> Vertices
    template <typename T, typename EdgeType>
        auto graph_boundary_matrix(const Eigen::MatrixBase<EdgeType>& E) -> Eigen::SparseMatrix<T> {



            std::vector<Eigen::Triplet<T>> trips;
            for(int i = 0; i < E.cols(); ++i) {
                auto e = E.col(i);
                trips.emplace_back(e(0),i,-1);
                trips.emplace_back(e(1),i,1);
            }
            Eigen::SparseMatrix<T> A(E.maxCoeff()+1,E.cols());
            A.setFromTriplets(trips.begin(),trips.end());
            return A;
        }

}}}


#endif//BOUNDARY_MATRIX_H
