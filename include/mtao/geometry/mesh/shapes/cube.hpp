#pragma once
#include "mtao/types.hpp"
#include <iostream>


namespace mtao::geometry::mesh::shapes {
    namespace cube_internal {
        template <typename T> 
            struct vert_container {
                const static T _V[36];
            };

        template <>
                const float vert_container<float>::_V[36];
        template <>
                const double vert_container<double>::_V[36];
            extern const int _F[36];
    }

    // [0,1]^D cubes, E selects the embedding dimension
    // the cube selected is always the first D dimensions in the embedded space
    template <typename T, int D = 3, int E = 3>
        std::tuple<mtao::ColVectors<T,E>,mtao::ColVecs3i> cube() {
            auto _VM = Eigen::Map<const mtao::ColVectors<T,3>>(cube_internal::vert_container<T>::_V,3,12);
            auto _FM = Eigen::Map<const mtao::ColVecs3i>(cube_internal::_F,3,12);


            if constexpr(D == 2) {
                return {_VM.template topLeftCorner<E,4>(), _FM.template topLeftCorner<3,2>()};
            } else if constexpr(D == 3) {
                return {_VM.template topLeftCorner<E,8>(), _FM.template topLeftCorner<3,12>()};
            }

        }

    template <typename T, int E>
    auto square() { return cube<T, 2, E>(); }

}
