#pragma once
#include "hemisphere_impl.hpp"


namespace mtao { namespace geometry { namespace mesh {
template <typename T, int D=3>
    std::tuple<mtao::ColVectors<T,D>, mtao::ColVectors<int,D>> hemisphere(int depth) {
        if constexpr(D == 2) {//in 2d Depth is number of segments
            mtao::ColVectors<T,D> V(2,depth);
            mtao::ColVectors<int,D> E(2,depth-1);
            T pi = std::acos(-1);
            T dt = pi / depth;
            for(int i = 0; i < depth; ++i) {
                //edges are "backwards" to match area_normals using right hand rule to allow the normals be outward facing
                if(i < depth-1) {
                    E(1,i) = i;
                    E(0,i) = (i+1)%depth;
                }
                V(0,i) = std::cos(i * dt);
                V(1,i) = std::sin(i * dt);
            }
            return {V,E};

        } else { 
            detail::HemiSphereMeshFactory<T> smf(depth);
            return std::make_tuple(smf.V(),smf.F());
        }
}

}}}

