#include <openvdb/openvdb.h>
#include <openvdb/tools/VolumeToMesh.h>

#include <cxxopts.hpp>
#include <mtao/iterator/enumerate.hpp>
#include <mtao/iterator/shell.hpp>

openvdb::GridBase::Ptr get_grid(const std::string &filename,
                                const std::string &gridname = "") {
    openvdb::io::File file(filename);
    file.open();
    for (openvdb::io::File::NameIterator nameIter = file.beginName();
         nameIter != file.endName();
         ++nameIter) {
        if (gridname.empty() || nameIter.gridName() == gridname) {
            return file.readGrid(nameIter.gridName());
        } else {
            std::cout << "skipping grid " << nameIter.gridName() << std::endl;
        }
    }
    if (!gridname.empty()) {
        std::cout << "WARN: did not find the grid " << gridname << " in "
                  << filename << std::endl;
    } else {
        std::cout << "WARN: did not find a grid in " << filename << std::endl;
    }
    return {};
}

int main(int argc, char *argv[]) {
    cxxopts::Options options("test", "A brief description");

    options.add_options()("vdb_file", "vdb grid file", cxxopts::value<std::string>())(
      "output", "output mesh file", cxxopts::value<std::string>())(
      "n,name", "name of the grid in the vdb_file", cxxopts::value<std::string>())(
      "t,type", "output file type [obj,ply]", cxxopts::value<std::string>()->default_value("obj"))(
      "i,isolevel", "isolevel to mesh against", cxxopts::value<double>()->default_value("0.0"))(
      "m,mask", "surface mask (another vdb grid)", cxxopts::value<std::string>())(
      "mask_name", "the name of the mask grid in the mask vdb file", cxxopts::value<std::string>())(
      "a,adaptivity", "spacial adaptivity", cxxopts::value<double>()->default_value("0.0"))(
      "f,adaptivity_file", "spacial adaptivity(another vdb grid)", cxxopts::value<std::string>())(
      "adaptivity_name",
      "the name of the adaptivity grid in the adaptivity vdb file",
      cxxopts::value<std::string>())(
      "q,quads", "Output quads (only for obj)", cxxopts::value<bool>()->default_value("false"))(
      "r,relax_tris", "Relax disoriented triangles", cxxopts::value<bool>()->default_value("true"))("h,help", "Print usage");
    options.parse_positional({ "vdb_file", "output" });
    options.positional_help({ "<vdb_file> <output_mesh>" });

    auto result = options.parse(argc, argv);

    bool help_out = result.count("help");
    if (!bool(result.count("vdb_file"))) {
        std::cout << "vdb_file required!" << std::endl;
        help_out = true;
    }
    if (help_out) {
        std::cout << options.help() << std::endl;
        exit(0);
    }
    // std::string vdb_file = result["vdb_file"].as<std::string>();
    bool quads = result["quads"].as<bool>();
    bool relaxDisorientedTriangles = result["relax_tris"].as<bool>();
    std::string vdb_file = result["vdb_file"].as<std::string>();
    std::string type = result["type"].as<std::string>();
    double isolevel = result["isolevel"].as<double>();
    double adaptivity = result["adaptivity"].as<double>();

    openvdb::initialize();
    std::cout << "Starting to do things" << std::endl;

    openvdb::GridBase::Ptr grid = nullptr;
    openvdb::GridBase::Ptr mask_grid = nullptr;
    openvdb::GridBase::Ptr adaptivity_grid = nullptr;

    std::cout << "Checking for my own name and fetching myself" << std::endl;
    if (result.count("name")) {
        std::string name = result["name"].as<std::string>();
        grid = get_grid(vdb_file, name);
    } else {
        grid = get_grid(vdb_file);
    }

    std::cout << "Checking for my mask name and fetching mask" << std::endl;
    if (result.count("mask")) {
        std::string mask = result["mask"].as<std::string>();
        if (result.count("mask_name")) {
            std::string mask_name = result["mask_name"].as<std::string>();
            mask_grid = get_grid(mask, mask_name);
        } else {
            mask_grid = get_grid(mask);
        }
    }

    std::cout << "Checking for adaptivity name and fetching adaptivity"
              << std::endl;
    if (result.count("adaptivity_file")) {
        std::string adaptivity_file =
          result["adaptivity_file"].as<std::string>();
        if (result.count("adaptivity_name")) {
            std::string adaptivity_name =
              result["adaptivity_name"].as<std::string>();
            adaptivity_grid = get_grid(adaptivity_file, adaptivity_name);
        } else {
            adaptivity_grid = get_grid(adaptivity_file);
        }
    }

    // openvdb::GridBase::Ptr adaptivity
    // if (result.count("bar")) bar = result["bar"].as<std::string>();

    auto run = [&](auto &&grid) {
        using GridType = std::decay_t<decltype(grid)>;
        using Scalar = GridType::ValueType;
        openvdb::tools::VolumeToMesh vtm(isolevel, adaptivity, relaxDisorientedTriangles);
        if (mask_grid) {
            vtm.setSurfaceMask(mask_grid);
        }
        if (adaptivity_grid) {
            vtm.setSpatialAdaptivity(adaptivity_grid);
        }

        vtm(*grid);
        mtao::ColVectors<Scalar, 3> V(3, vtm.pointListSize());
        auto pts = vtm.pointList();
        for (auto &&[idx, pt] :
             mtao::iterator::enumerate(mtao::iterator::shell(pts))) {
        }
        mtao::ColVectors<Scalar, 3> V(3, vtm.pointListSize());
        auto pts = vtm.pointList();
        for (auto &&[idx, pt] :
             mtao::iterator::enumerate(mtao::iterator::shell(pts))) {
        }
    };

    if (grid->type() == "Tree_float_5_4_3") {
        run(std::static_pointer_cast<openvdb::FloatGrid>(grid));
    } else if (grid->type() == "Tree_double_5_4_3") {
        run(std::static_pointer_cast<openvdb::DoubleGrid>(grid));

    } else {
        std::cerr << "Unknown tree type! pass in float or double grids!: "
                  << grid->type() << std::endl;
    }

    return 0;
}
