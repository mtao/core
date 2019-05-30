#pragma once

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <set>

#include <mtao/types.h>

namespace mtao { namespace geometry { namespace mesh {

    //Only for simplices
    template <typename CellType>
auto boundary_facets(const Eigen::MatrixBase<CellType>& C) {


    constexpr static int CellSize = CellType::RowsAtCompileTime;
    constexpr static int FacetSize = (CellSize<1)?CellSize:(CellSize - 1);
    using FacetType = mtao::ColVectors<typename CellType::Scalar,FacetSize>;

    using Index = typename CellType::Scalar;

    using FacetArray = typename std::array<Index,FacetSize>;
    using CellArray = typename std::array<Index,CellSize>;
    
    using FacetVector = typename mtao::Vector<Index,FacetSize>;
    using CellVector = typename mtao::Vector<Index,CellSize>;

    std::set<std::array<Index,FacetSize>> facet_set;

    FacetArray f;
    Eigen::Map<FacetVector> fm(f.data());

    FacetArray f2;
    Eigen::Map<FacetVector> fm2(f2.data());
    CellArray c;
    Eigen::Map<CellVector> cm(c.data());
    for(int i = 0; i < C.cols(); ++i) {
        auto c = C.col(i);
        fm = c.template topRows<FacetSize>();
        for(int j = 0; j < CellSize; ++j) {
            fm2 = fm;
            std::sort(f2.begin(),f2.end());
            facet_set.insert(f2);

            auto unused = (j + FacetSize)%CellSize;
            fm(j%FacetSize) = c(unused);

        }
    }

    FacetType F(C.rows()-1,facet_set.size());
    int idx = 0;
    for(auto&& f: facet_set) {
        F.col(idx++) = Eigen::Map<const FacetVector>(f.begin());
    }
    return F;
}


}}}

