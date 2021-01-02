#pragma once
#include "mtao/types.hpp"
#include "mtao/geometry/grid/grid.h"

//Util for visualizing grids as triangle meshes


namespace mtao {
namespace geometry {
    namespace grid {
        template<typename GridType>
        class GridTriangulator {
          public:
            constexpr static int D = GridType::D;
            using T = typename GridType::Scalar;

          public:
            GridTriangulator(const GridType &g) : g(g) {}
            ColVectors<T, D> vertices() const;
            using coord_type = typename GridType::coord_type;
            ColVectors<int, 3> faces() const;
            ColVectors<int, 2> edges() const;

          private:
            const GridType &g;
        };
    }// namespace grid
}// namespace geometry
}// namespace mtao

#include "triangulation_impl.hpp"
