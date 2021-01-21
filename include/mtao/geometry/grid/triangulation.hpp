#pragma once
#include "mtao/types.hpp"
#include "mtao/geometry/grid/grid.h"

//Util for visualizing grids as triangle meshes


namespace mtao::geometry::grid {
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

    std::array<size_t, 4> face_loop(const std::array<size_t, D> &coord, size_t dim = 0) const;
    std::array<size_t, 4> face_loop(size_t index) const;

  private:
    const GridType &g;
};
}// namespace mtao::geometry::grid

#include "triangulation_impl.hpp"
