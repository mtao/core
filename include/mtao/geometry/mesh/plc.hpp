#pragma once
#include "mtao/types.h"

namespace mtao { namespace geometry { namespace mesh {

    namespace plc {
    template <int D>
    using IndexType = mtao::ColVectors<int,D>;
    using MarkerType = mtao::VectorX<int>;
    }
}
