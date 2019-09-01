#pragma once 
#include <mtao/geometry/mesh/shapes/tube.hpp>
#include <mtao/geometry/mesh/shapes/cone.hpp>

namespace mtao::geometry::mesh::shapes {

    template <typename T>
        std::tuple<mtao::ColVectors<T,3>,mtao::ColVecs3i> arrow(const mtao::ColVectors<T,3>& P, double cone_radius, int N = 5, double arrow_radius_scale = 1.1, double tip_scale=1.) {
            auto [tV,tF] = mtao::geometry::mesh::shapes::capped_tube(P,cone_radius,N);
            auto [bV,bF] = mtao::geometry::mesh::shapes::cone((tip_scale * (P.col(P.cols()-1) - P.col(P.cols()-2))).eval(),arrow_radius_scale * cone_radius ,N);

            bV.colwise() += P.col(P.cols()-1);

            return {
                mtao::eigen::hstack(tV,bV),
                    mtao::eigen::hstack(tF,bF.array()+tV.cols())
            };

        }

}
