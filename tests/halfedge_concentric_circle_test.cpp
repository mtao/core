#include <iostream>
#include "mtao/geometry/mesh/sphere.hpp"
#include "mtao/geometry/mesh/halfedge.hpp"
#include "mtao/data_structures/disjoint_set.hpp"
#include <mtao/eigen/stack.h>


#include "mtao/logging/logger.hpp"
using namespace mtao::logging;


int main(int argc, char *argv[]) {

    using namespace mtao::geometry::mesh;
    int N = 5;
    auto [V, E] = sphere<float, 2>(5);
    V = mtao::eigen::hstack(V, 2 * V, 3 * V);
    E = mtao::eigen::hstack(E, E.array() + N, E.array() + 2 * N);

    EmbeddedHalfEdgeMesh<float, 2> hem = EmbeddedHalfEdgeMesh<float, 2>::from_edges(V, E);
    hem.make_topology();
    hem.tie_nonsimple_cells();
    std::cout << std::endl
              << std::endl;

    std::cout << V << std::endl;
    std::cout << E << std::endl;

    std::cout << hem.edges() << std::endl;

    int NI = 40, NJ = 40;
    auto HES = hem.cell_halfedges();


    auto print_g = [&](auto &&g) {
        for (auto &&vj : g) {
            for (auto &&v : vj) {
                if (v < 0) {
                    std::cout << ".";
                } else {
                    std::cout << v;
                }
            }
            std::cout << std::endl;
        }
    };
    std::vector<std::vector<int>> D(NI, std::vector<int>(NJ, 0));
    for (int i = 0; i < NI; ++i) {
        for (int j = 0; j < NJ; ++j) {
            mtao::Vec2f p(6 * float(i) / (NI - 1), 6 * float(j) / (NJ - 1));
            p.array() -= 3;
            for (auto &&he : HES) {
                D[i][j] += (hem.is_inside(he, p) ? 1 : 0);
                //std::cout << hem.winding_number(hem.edge(he),p) << ": " << hem.signed_area(hem.edge(he)) << std::endl;
            }
        }
    }
    print_g(D);
    for (int i = 0; i < NI; ++i) {
        for (int j = 0; j < NJ; ++j) {
            mtao::Vec2f p(6 * float(i) / (NI - 1), 6 * float(j) / (NJ - 1));
            p.array() -= 3;
            D[i][j] = hem.get_cell(p);
        }
    }
    print_g(D);
    auto C = hem.cells_multi_component_map();

    for (auto &&[cid, cs] : C) {
        std::cout << cid << ": ";
        for (auto &&c : cs) {
            std::cout << "[";
            for (auto &&v : c) {
                std::cout << v << ",";
            }
            std::cout << "]";
        }
        std::cout << std::endl;
    }
    return 0;
}
