#include <mtao/geometry/point_cloud/partio_loader.hpp>
#include <iostream>
#include <fstream>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <mtao/eigen/stack.hpp>
#include <mtao/geometry/mesh/write_obj.hpp>
#include <cxxopts.hpp>
#include <fmt/format.h>


int main(int argc, char *argv[]) {

    cxxopts::Options options("test", "A brief description");

    // clang-format: off
    options.add_options()("input_format", "fmt-style format string for files", cxxopts::value<std::string>()->default_value("output-{:04}.bgeo.gz"))(
      "output_format", "output fmt-style format", cxxopts::value<std::string>()->default_value("output-{:04}.obj"))(
      "count", "number of segments to use for segment counts", cxxopts::value<int>()->default_value("5"))(
      "d,dimension", "dimension to slice", cxxopts::value<int>()->default_value("-1"))(
      "t,thickness", "slice thickness", cxxopts::value<double>()->default_value("-1"))(
      "s,slice-center", "center of the slice", cxxopts::value<double>()->default_value("0.0"))(
      "f,filter-lasers", "the max distance a point can travel in a single step", cxxopts::value<double>()->default_value("0.1"))(
      "test", "tests which files are available with the given format")(
      "avoid-overwriting", "avoid overwriting existing files")(
      "h,help", "Print usage");
    // clang-format: on
    options.parse_positional({ "input_format", "output_format" });
    options.positional_help({ "<input_format> <output_format>" });

    auto result = options.parse(argc, argv);

    bool help_out = result.count("help");
    bool test_files = result.count("test");
    bool avoid_overwritting = result.count("avoid-overwriting");
    if (!bool(result.count("input_format"))) {
        help_out = true;
    }
    if (!bool(result.count("output_format"))) {
        help_out = true;
    }
    if (help_out) {
        std::cout << options.help() << std::endl;
        exit(0);
    }
    std::list<std::string> paths;


    std::string input_format = result["input_format"].as<std::string>();
    std::string output_format = result["output_format"].as<std::string>();
    int dim = result["dimension"].as<int>();
    int count = result["count"].as<int>();
    double thickness = result["thickness"].as<double>();
    double center = result["slice-center"].as<double>();
    double lasers_filter = result["filter-lasers"].as<double>();

    //std::vector<std::filesystem::path> input_files;
    if (test_files) {
        spdlog::info("Just testing for available files with the format");
    }
    int max_file_index = 0;
    int first_output_file = 0;
    for (max_file_index = 0;; ++max_file_index) {
        std::string input_path_str = fmt::format(input_format, max_file_index);
        std::filesystem::path current_input_path = input_path_str;

        if (!std::filesystem::exists(current_input_path)) {
            break;
        } else {

            if (test_files) {
                spdlog::info("{}", std::string(current_input_path));
            } else if (avoid_overwritting) {
                // if we're not overwriting then figure out when we need to start caching
                std::string output_path_str = fmt::format(output_format, max_file_index);
                std::filesystem::path current_output_path = output_path_str;
                spdlog::info("Checking for overwrites in {} ({})", std::string(current_input_path), output_path_str);
                if (!std::filesystem::exists(current_output_path)) {
                    spdlog::info("Found an input file without an output file, {}", max_file_index);
                    first_output_file = max_file_index;
                }
            }
        }
    }
    if (test_files) {
        return 0;
    }
    std::vector<mtao::ColVecs3d> cache(count);
    std::vector<std::string> cache_filenames(count);
    bool do_slice = (dim >= -1) && (thickness > 0);

    int starting_index = avoid_overwritting ? std::max<int>(0, first_output_file - count) : 0;
    spdlog::info("Going to go through {} frames starting at {}", max_file_index, starting_index);
    for (int index = starting_index ; index < max_file_index; ++index) {
        spdlog::info("Making file for obj {}", index);
        int min_index = std::max<int>(0, index - count + 1);
        // load the newest file
        int cache_index = index % count;
        std::string input_path_str = fmt::format(input_format, index);
        std::string output_path_str = fmt::format(output_format, index);
        std::filesystem::path current_input_path = input_path_str;
        std::filesystem::path current_output_path = output_path_str;
        spdlog::info("Adding {} to cache element {}", input_path_str, cache_index);
        auto &P = cache[cache_index] = mtao::geometry::point_cloud::points_from_partio(std::filesystem::absolute(current_input_path));
        cache_filenames[cache_index] = current_input_path;

        if (avoid_overwritting && std::filesystem::exists(current_output_path)) {
            continue;
        }
        std::ofstream ofs(current_output_path);

        int global_index = 1;
        spdlog::info("I have {} points to check out", P.size());
        for (int pidx = 0; pidx < P.cols(); ++pidx) {
            bool use_it = true;
            if (do_slice) {
                auto p = P.col(pidx);
                double v = p(dim);
                if (std::abs(v - center) > thickness) {
                    use_it = false;
                }
            }

            if (use_it) {
                int global_stop = index - min_index + global_index;
                mtao::ColVecs3d pts(3, count);
                int ptssize = 0;
                for (int subindex = min_index; subindex <= index; ++subindex) {
                    int cache_index = subindex % count;
                    //std::cout << cache_filenames[cache_index] << " ";
                    const auto &P = cache[cache_index];
                    if (pidx < P.cols()) {
                        pts.col(ptssize++) = P.col(pidx);
                        //ofs << "v " << P.col(pidx).transpose() << std::endl;
                    } else {
                        global_stop--;
                    }
                }
                if (pts.size() == 1) {
                    use_it = false;
                }

                for (int j = 0; j < ptssize - 1; ++j) {
                    auto a = pts.col(j);
                    auto b = pts.col(j + 1);
                    if ((a - b).norm() > lasers_filter) {
                        use_it = false;
                    }
                }
                if (use_it) {
                    for (int j = 0; j < ptssize; ++j) {
                        ofs << "v " << pts.col(j).transpose() << "\n";
                    }
                    //spdlog::info("line indices: {} => {}", global_index, global_stop);
                    ofs << "l ";
                    for (; global_index <= global_stop; ++global_index) {
                        //std::cout << global_index << std::endl;
                        ofs << global_index << " ";
                    }
                    ofs << std::endl;
                    //std::cout << std::endl;
                }
            }
        }
    }
    return 0;
}
