#include <iostream>
#include <mtao/geometry/mesh/shapes/cube.hpp>

int main(int argc ,char* argv[]) {
    {
        auto [V,F] = mtao::geometry::mesh::shapes::cube<float,2,2>();
        std::cout << V << std::endl;
        std::cout << F << std::endl;
    }
    {
        auto [V,F] = mtao::geometry::mesh::shapes::square<float,2>();
        std::cout << V << std::endl;
        std::cout << F << std::endl;
    }
    std::cout << std::endl;
    {
        auto [V,F] = mtao::geometry::mesh::shapes::cube<float,2,3>();
        std::cout << V << std::endl;
        std::cout << F << std::endl;
    }
    {
        auto [V,F] = mtao::geometry::mesh::shapes::square<float,3>();
        std::cout << V << std::endl;
        std::cout << F << std::endl;
    }
    std::cout << std::endl;
    {
        auto [V,F] = mtao::geometry::mesh::shapes::cube<float,3,3>();
        std::cout << V << std::endl;
        std::cout << F << std::endl;
    }

}
