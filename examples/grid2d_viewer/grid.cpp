#include "grid.h"
#include <array>
#include "mtao/geometry/grid/indexers/ordered_indexer.hpp"
#include "mtao/geometry/grid/triangulation.hpp"



using namespace mtao::geometry::grid::indexing;

mtao::ColVectors<float,2> make_vertices(const int i, const int j) {
    mtao::geometry::grid::GridTriangulation<float,OrderedIndexer<2>> g({{i,j}});
    mtao::ColVectors<float,2> V(2,i*j);
    for(int ii = 0; ii < i; ++ii) {
        for(int jj = 0; jj < j; ++jj) {
            V.col(g.index(ii,jj)) = mtao::Vector<float,2>(static_cast<float>(ii)/(i-1),static_cast<float>(jj)/(j-1));
        }
    }
    return g.vertices();
}
mtao::ColVectors<unsigned int,3> make_topology(const int i, const int j) {
    mtao::geometry::grid::GridTopologicalTriangulation<OrderedIndexer<2>> g({{i,j}});
    return g.faces();
}
std::tuple<mtao::ColVectors<float,2>,mtao::ColVectors<unsigned int,3>> make_mesh(const int i, const int j) {
    return {make_vertices(i,j),make_topology(i,j)};
}
