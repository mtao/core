#ifndef _SPHERE_MESH_GEN_H_
#define _SPHERE_MESH_GEN_H_
#include "sphere_ipl.hpp"


namespace mtao { namespace geometry { namespace mesh {
template <typename T>
std::tuple<mtao::ColVectors<T,3>, mtao::ColVectors<int,3>> sphere(int depth) {
    detail::SphereMeshFactory<T> smf(depth);
    return std::make_tuple(smf.V(),smf.F());
}

}}}

#endif
