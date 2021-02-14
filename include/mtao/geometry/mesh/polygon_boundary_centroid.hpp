#pragma once
#include <numeric>
#include <vector>
#include "mtao/types.hpp"
#include "mtao/eigen/shape_checks.hpp"
#include "mtao/geometry/mesh/polygon_boundary.hpp"
#include "mtao/geometry/volume.hpp"
#include "mtao/geometry/centroid.hpp"



namespace mtao::geometry::mesh {


template<eigen::concepts::ColVecs2Compatible ColVecs>
auto centroid(const PolygonBoundaryIndices &PBI, const ColVecs &V) {
    using RetType = std::decay_t<decltype(V.col(0).eval())>;
    using Scalar = typename ColVecs::Scalar;

    // it is at least a vec type
    RetType mass = RetType::Zero(2);
    Scalar vol = 0;

    auto run = [&](const std::vector<int>& curve) {
        Scalar v = curve_volume(V,curve);
        mass += v * curve_centroid(V,curve);
        vol += v;

    };
    run(PBI.outer_loop());
    for(auto&& l: PBI.holes) {
        run(l);
    }

    return RetType(mass / vol);


}

}// namespace mtao::geometry::mesh
