#include <mtao/geometry/point_cloud/partio_loader.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#include <mtao/eigen/stack.hpp>
#include <mtao/geometry/mesh/write_obj.hpp>
#include <cxxopts.hpp>


int main(int argc, char *argv[]) {

    cxxopts::Options options("test", "A brief description");

    // clang-format: off
    options.add_options()("input_file", "input partio filename", cxxopts::value<std::string>())(
      "output_file", "output obj filename", cxxopts::value<std::string>())(
      "count", "number of segments to use for segment counts", cxxopts::value<int>()->default_value("5"))(
      "d,dimension", "dimension to slice", cxxopts::value<int>()->default_value("-1"))(
      "t,thickness", "slice thickness", cxxopts::value<double>()->default_value("-1"))(
      "s,slice-center", "center of the slice", cxxopts::value<double>()->default_value("0.0"))(
      "f,filter-lasers", "the max distance a point can travel in a single step", cxxopts::value<double>()->default_value("0.1"))(
      "test", "tests which files are available with the given format")(
      "avoid-overwriting", "avoid overwriting existing files")(
      "h,help", "Print usage");
    // clang-format: on
    options.parse_positional({ "input_file", "output_file" });
    options.positional_help({ "<input_file> <output_file>" });

    auto result = options.parse(argc, argv);

    bool help_out = result.count("help");
    bool test_files = result.count("test");
    bool avoid_overwritting = result.count("avoid-overwriting");
    if (!bool(result.count("input_file"))) {
        help_out = true;
    }
    if (!bool(result.count("output_file"))) {
        help_out = true;
    }
    if (help_out) {
        std::cout << options.help() << std::endl;
        exit(0);
    }
    std::list<std::string> paths;


    std::string input_file = result["input_file"].as<std::string>();
    std::string output_file = result["output_file"].as<std::string>();
    int dim = result["dimension"].as<int>();
    int count = result["count"].as<int>();
    double thickness = result["thickness"].as<double>();
    double center = result["slice-center"].as<double>();
    double lasers_filter = result["filter-lasers"].as<double>();

    //std::vector<std::filesystem::path> input_files;
    if (test_files) {
        spdlog::info("Just testing for available files with the format");
    }
    const std::string input_filename = argv[1];
    const std::string output_filename = argv[2];
    //if (!mtao::geometry::point_cloud::partio_has_velocity(input_filename)) {
        auto P = mtao::geometry::point_cloud::points_from_partio(input_filename);
        mtao::geometry::mesh::write_objD(P, mtao::ColVecs2i{}, output_filename);

        /*
    } else {
        spdlog::info("Showing particles with velocities");
        auto [P, V] = mtao::geometry::point_cloud::points_and_velocity_from_partio(input_filename);


        int stride = P.cols();

        mtao::ColVecs2i E(2, stride);
        E.row(0) = mtao::VecXi::LinSpaced(stride, 0, stride - 1).transpose();
        E.row(1) = stride + E.row(0).array();


        mtao::geometry::mesh::write_objD(mtao::eigen::hstack(P, P + V), E, output_filename);
    }
    */
}
