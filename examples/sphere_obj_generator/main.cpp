#include <iostream>
#include <mtao/cmdline_parser.hpp>
#include <mtao/colvector_loop.hpp>
#include <mtao/geometry/mesh/sphere.hpp>
#include <mtao/logging/logger.hpp>
int main(int argc, char* argv[]) {

    mtao::logging::active_loggers["default"].set_level(mtao::logging::Level::Warn);


    mtao::CommandLineParser clp;
    clp.add_option("radius", 1.0);
    clp.add_option("depth", 3);
    clp.parse(argc,argv);

    double radius;
    int depth;
    if(clp.args().size() > 0) {
        radius = std::stof(clp.arg(0));
        depth = std::stoi(clp.arg(1));
    } else {
        radius = clp.optT<double>("radius");
        depth = clp.optT<int>("depth");
    }


    auto [V,F] = mtao::geometry::mesh::sphere<double,3>(depth);


    for(auto&& v: mtao::colvector_loop(V)) {
        std::cout << "v " << radius * v.transpose() << std::endl;
    }
    for(auto&& f: mtao::colvector_loop(F)) {
        std::cout << "f " << (f.array().transpose()+1) << std::endl;
    }

}
