#include <mtao/geometry/mesh/read_obj.hpp>
#include <mtao/geometry/mesh/write_obj.hpp>
#include <mtao/geometry/mesh/boundary_facets.h>
#include <mtao/geometry/mesh/boundary_elements.h>
#include <cxxopts.hpp>
#include <mtao/iterator/enumerate.hpp>


int main(int argc, char * argv[]) {
    cxxopts::Options options("add_boundary_to_obj", "append lines to an obj for its boundary");
// clang-format off
    options.add_options()
        ("obj_file", "input obj filename", cxxopts::value<std::string>())
        ("output", "output obj filename", cxxopts::value<std::string>())
        ("t,triangles", "show triangles")
        ("h,help", "Print usage");
// clang-format on
    options.parse_positional({"obj_file","output"});
    auto result = options.parse(argc, argv);

    bool help_out = result.count("help") > 0;
    if (help_out) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    bool show_triangles = result.count("triangles") > 0;

    std::string mesh_file = result["obj_file"].as<std::string>();
    std::string output = result["output"].as<std::string>();

    auto [V,F] = mtao::geometry::mesh::read_objD(mesh_file);

    auto E = mtao::geometry::mesh::boundary_facets(F);

    auto BEI = mtao::geometry::mesh::boundary_element_indices(F,E);


    mtao::ColVecs2i BE(2,BEI.size());
    for(auto&& [idx, eidx]: mtao::iterator::enumerate(BEI)) {
        BE.col(idx) = E.col(eidx);
    }

    if(show_triangles) {
    mtao::geometry::mesh::write_objD(V,F,BE, output);
    } else {
    mtao::geometry::mesh::write_objD(V,{},BE, output);
    }
    return 0;
}
