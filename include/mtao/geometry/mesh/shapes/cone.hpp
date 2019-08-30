
#pragma once
#include "mtao/geometry/mesh/shapes/disc.hpp"


namespace mtao::geometry::mesh::shapes {

    template <typename T>
        std::tuple<mtao::ColVectors<T,3>,mtao::ColVecs3i> cone(const mtao::Vector<T,3>& D, double radius = 1, int N = 5) {
            using Vec = mtao::Vector<T,3>;
            Vec U = D.cross(Vec::Unit(0));
            if(u.norm() < 1e-5) {
                U = D.cross(Vec::Unit(1));
            }
            Vec V = T.cross(U);
            auto [tV,tF] = mtao::geometry::mesh::shapes::disc<true>(U,V,N);
            auto [bV,bF] = mtao::geometry::mesh::shapes::disc<false>(U,V,N);
            tV = D;

            return {
                mtao::eigen::hstack(tV,bV),
                    mtao::eigen::hstack(tF,bF.array()+tV.cols())
            };

        }

}
