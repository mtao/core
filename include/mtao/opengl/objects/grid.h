#pragma once
#include "mesh.h"
#include "mtao/geometry/grid/grid.h"
#include <type_traits>
#include "mtao/geometry/grid/triangulation.hpp"


namespace mtao::opengl::objects {

template<int D>
class Grid : public Mesh<D> {
  public:
      // this is the underlying actual grid type used
    using GridType = typename mtao::geometry::grid::GridD<float, D>;
    Grid() = default;
    Grid(const GridType &g) { set(g); }
    Grid(Grid &&) = default;
    Grid &operator=(Grid &&) = default;
    void set(const GridType &g) {
        mtao::geometry::grid::GridTriangulator<std::decay_t<decltype(g)>> gt(g);
        auto V = gt.vertices();
        Mesh<D>::setTriangleBuffer(V, gt.faces().template cast<unsigned int>());
        Mesh<D>::setEdgeBuffer(gt.edges().template cast<unsigned int>());
    }
};
}// namespace mtao::opengl::objects
