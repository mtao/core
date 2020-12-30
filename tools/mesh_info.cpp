#include <iostream>
#include <mtao/geometry/mesh/read_obj.hpp>
#include <mtao/geometry/bounding_box.hpp>

int main(int argc, char *argv[]) {
    std::string filename = argv[1];
    auto [V, F] = mtao::geometry::mesh::read_objD(filename);
    std::cout << "#V,#F: " << V.cols() << "," << F.cols() << std::endl;
    auto bb = mtao::geometry::bounding_box(V);
    std::cout << "Bounding box: " << bb.min().transpose() << " => " << bb.max().transpose() << std::endl;
}
