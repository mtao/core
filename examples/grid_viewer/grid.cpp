#include "grid.h"
#include <array>
#include "mtao/geometry/grid/indexers/ordered_indexer.hpp"
#include "mtao/geometry/grid/triangulation.hpp"


using namespace mtao::geometry::grid::indexing;
mtao::ColVectors<float,3> make_vertices(const int i, const int j, const int k) {
    mtao::geometry::grid::GridTriangulation<float,OrderedIndexer<3>> g({{i,j,k}});
    return g.vertices();
}
mtao::ColVectors<unsigned int,2> make_edge_topology(const int i, const int j, const int k) {
    mtao::geometry::grid::GridTopologicalTriangulation<OrderedIndexer<3>> g({{i,j,k}});
    return g.edges();
}
mtao::ColVectors<unsigned int,3> make_topology(const int i, const int j, const int k) {
    mtao::geometry::grid::GridTopologicalTriangulation<OrderedIndexer<3>> g({{i,j,k}});
    return g.faces();
}
std::tuple<mtao::ColVectors<float,3>,mtao::ColVectors<unsigned int,3>> make_mesh(const int i, const int j, const int k) {
    return {make_vertices(i,j,k),make_topology(i,j,k)};
}
