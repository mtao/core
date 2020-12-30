#include <iostream>
#include <mtao/geometry/mesh/laplacian.hpp>

int main() {
    {
        mtao::ColVectors<float, 2> V(2, 4);
        V << mtao::Vec2f(0., 0.), mtao::Vec2f(1., 0.), mtao::Vec2f(1., 1.), mtao::Vec2f(0., 1.);
        mtao::ColVectors<int, 3> F(3, 2);
        F << 0, 0,
          1, 2,
          2, 3;

        std::cout << F << std::endl;
        std::cout << mtao::geometry::mesh::cot_laplacian(V, F) << std::endl;
        std::cout << mtao::geometry::mesh::strong_cot_laplacian(V, F) << std::endl;
    }
    {
        mtao::ColVectors<float, 3> V(3, 4);
        V << mtao::Vec3f(0., 0., 0.), mtao::Vec3f(1., 0., 0.), mtao::Vec3f(0., 1., 0.), mtao::Vec3f(0., 0., 1.);
        mtao::ColVectors<int, 4> F(4, 1);
        F << 0, 1, 2, 3;

        std::cout << F << std::endl;
        std::cout << mtao::geometry::mesh::cot_laplacian(V, F) << std::endl;
        std::cout << mtao::geometry::mesh::strong_cot_laplacian(V, F) << std::endl;
    }
}
