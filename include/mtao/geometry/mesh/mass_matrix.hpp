#pragma once

#include "mtao/geometry/volume.h"

namespace mtao { namespace geometry { namespace mesh {

    template <typename VertexDerived, typename SimplexDerived> 
        auto mass_matrix( const Eigen::MatrixBase<VertexDerived>& V, const Eigen::MatrixBase<SimplexDerived>& S) {
            return dual_volumes(V,S);
        }

}}}
