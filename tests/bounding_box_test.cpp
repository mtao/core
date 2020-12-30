#include "mtao/geometry/bounding_box.hpp"
#include "mtao/geometry/bounding_box_mesh.hpp"
#include "mtao/logging/timer.hpp"
#include <iostream>


int main() {
    mtao::ColVectors<float, 3> P;
    for (int i = 0; i < 20; ++i) {
        {
            auto t = mtao::logging::timer("making data", true);
            P.resize(3, 2 << i);
            P.setRandom();
        }
        {
            auto t = mtao::logging::timer("bb1", true);
            auto bb = mtao::geometry::bounding_box(P);
            auto [V, F, E] = mtao::geometry::bounding_box_mesh<true>(bb);
            std::cout << V << std::endl;
            std::cout << F << std::endl;
            std::cout << E << std::endl;
        }
        {
            auto t = mtao::logging::timer("bb2", true);
            auto bb = mtao::geometry::bounding_box_slow(P);
        }
    }
}
