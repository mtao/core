#include <iostream>
#include <mtao/geometry/mesh/read_obj.hpp>
#include <mtao/geometry/mesh/write_ply.hpp>
#include <mtao/logging/logger.hpp>
int main(int argc, char* argv[]) {

    if(argc < 3) {
        std::cout << "obj2ply <objfile> <plyfile>" << std::endl;
        return 1;
    }

    auto [V,F] = mtao::geometry::mesh::read_objD(argv[1]);

    mtao::ColVecs3d C = V;
    for(int i = 0; i < 3; ++i) {
        auto c = C.row(i);
        c.array() -= c.minCoeff();
        c /= c.maxCoeff();
    }

    mtao::geometry::mesh::write_plyD(V,C,F,argv[2]);
    return 0;
}
