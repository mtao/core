#include <cxxopts.hpp>
#include <openvdb/openvdb.h>
#include <openvdb/tools/MeshToVolume.h>
#include <mtao/iterator/enumerate.hpp>
#include <mtao/iterator/shell.hpp>
#include <mtao/geometry/mesh/read_obj.hpp>
//#include <mtao/geometry/mesh/read_ply.hpp>



int main(int argc, char* argv[]) {
    cxxopts::Options options("test", "A brief description");

    options.add_options()
        ("mesh_file", "mesh grid file",cxxopts::value<std::string>())
        ("output", "output vdb file",cxxopts::value<std::string>())
        ("n,name", "name of the grid in the vdb_file",cxxopts::value<std::string>()->default_value("grid"))
        ("t,type", "output vdb grid [sdf,df,levelset]",cxxopts::value<std::string>()->default_value("sdf"))
        ("f,float_type", "output type of the vdb grid [float,double]" ,cxxopts::value<std::string>()->default_value("float"))
        ("w,half-width", "half-width of levelsets, for distance functions this is also the bandwidth",cxxopts::value<double>()->default_value("3"))
        ("i,interior_width", "Interior band-width for the narrow band",cxxopts::value<double>()->default_value("3"))
        ("e,exterior_width", "Exterior band-width for the narrow band",cxxopts::value<double>()->default_value("3"))
        ("v,voxel_size", "Size of a voxel",cxxopts::value<double>()->default_value(".5"))
        ("h,help", "Print usage");
    options.parse_positional({"mesh_file","output"});
    options.positional_help({"<mesh_file> <output_grid>"});

    auto result = options.parse(argc, argv);

    bool help_out = result.count("help");
    if(!bool(result.count("mesh_file"))) {
        std::cout << "mesh_file required!" << std::endl;
        help_out = true;
    }
    if (help_out) {
        std::cout << options.help() << std::endl;
        exit(0);
    }
    //std::string vdb_file = result["vdb_file"].as<std::string>();
    std::string mesh_file = result["mesh_file"].as<std::string>();
    std::string output = result["output"].as<std::string>();
    std::string name = result["name"].as<std::string>();
    std::string type = result["type"].as<std::string>();
    std::string float_type = result["float_type"].as<std::string>();
    double half_width = result["half-width"].as<double>();
    double interior_band = result["interior_width"].as<double>();
    double exterior_band = result["exterior_width"].as<double>();
    double voxel_size = result["voxel_size"].as<double>();

    std::string suffix = mesh_file.substr(mesh_file.size()-3,mesh_file.size());
    mtao::ColVecs3d V;
    mtao::ColVecs3i F;
    if(suffix  == "ply") {
        //std::tie(V,F) = mtao::geometry::mesh::read_plyD(mesh_file);
        std::cerr << "Ply reading currently broken, try again later" << std::endl;
        return 1;
    } else if(suffix == "obj") {
        std::tie(V,F) = mtao::geometry::mesh::read_objD(mesh_file);
    }

    auto transform = openvdb::math::Transform::createLinearTransform(/*voxel size=*/voxel_size);

    std::vector<openvdb::math::Vec3s> points(V.cols());
    std::vector<openvdb::Vec3I> triangles(F.cols());
    std::vector<openvdb::Vec4I> quads; // never really filled
    for(int i = 0; i < V.cols(); ++i) {
        auto v = V.col(i);
        points[i] = openvdb::math::Vec3s(v(0),v(1),v(2));
    }
    for(int i = 0; i < F.cols(); ++i) {
        auto f = F.col(i);
        triangles[i] = openvdb::Vec3I(f(0),f(1),f(2));
    }

    openvdb::initialize();
    std::cout << "Starting to do things" << std::endl;

    openvdb::GridBase::Ptr grid = nullptr;

    // use true_type/false_type to allow for cheap branching on whether to use a float or double grid
    auto make_grid = [&](auto t) {
        using GT = std::conditional_t<decltype(t)::value, openvdb::FloatGrid, openvdb::DoubleGrid>;
        if(type == "sdf") {
            grid = openvdb::tools::meshToSignedDistanceField<GT>(*transform, points, triangles, quads, interior_band, exterior_band);
        } else if(type == "df") {
            grid = openvdb::tools::meshToUnsignedDistanceField<GT>(*transform, points, triangles, quads, half_width);
        } else if(type == "levelset") {
            grid = openvdb::tools::meshToLevelSet<GT>(*transform, points, triangles, quads, half_width);
        }
    };
    if(float_type == "float") {
        make_grid(std::true_type());
    } else {
        make_grid(std::false_type());
    }
    //} else if(type == "volume") {
    //    grid = meshToVolume(*transform, points, triangles, quads, interior_band, exterior_band);

    grid->setName(name);

    openvdb::io::File file(output);
    file.write({grid});
    file.close();

    return 0;
}
