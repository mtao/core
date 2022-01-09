#include <iostream>
#include <cxxopts.hpp>
#include <fstream>
#include <optional>
#include <nlohmann/json.hpp>
#include <mtao/geometry/mesh/read_obj.hpp>
#include <mtao/geometry/mesh/boundary_facets.h>
#include <mtao/geometry/bounding_box.hpp>

int main(int argc, char *argv[]) {
    cxxopts::Options opts("mesh_view", "a simple mesh statistics tool");

    // clang-format off
    opts.add_options()
        ("filenames", "Arguments without options",cxxopts::value<std::vector<std::string>>())
        ("a,all", "show all statistics", cxxopts::value<bool>()->default_value("false"))
        ("j,json", "output as json", cxxopts::value<bool>()->default_value("false"))
        ("o,output", "output filepath", cxxopts::value<std::string>())
        ("c,counts", "show element counts", cxxopts::value<bool>()->default_value("false"))
        ("b,bounding_box", "show bounding box", cxxopts::value<bool>()->default_value("false"))
        ("h,help", "Print usage");

    opts.parse_positional({"filenames"});

    // clang-format on

    using json = nlohmann::json;
    auto res = opts.parse(argc, argv);

    if (res.count("help")) {
        std::cout << opts.help() << std::endl;
        return 0;
    }

    std::optional<std::ofstream> ofs_opt;
    if(res.count("output"))
    { 
        ofs_opt.emplace(res["output"].as<std::string>().c_str());
    }

    std::ostream& os = ofs_opt? *ofs_opt : std::cout;

    bool use_json = res["json"].as<bool>();
    bool do_all = res["all"].as<bool>();

#if 0
    std::vector<std::string> filenames;
    filenames.emplace_back(
      res["filenames"].as<std::string>());
#else
    auto filenames = res["filenames"].as<std::vector<std::string>>();
#endif
    for (auto &&filename : filenames) {
        json js;
        if (use_json) {
            js["filename"] = filename;
        }

        auto [V, F] = mtao::geometry::mesh::read_objD(filename);
        if (do_all || res["counts"].as<bool>()) {
            auto E = mtao::geometry::mesh::boundary_facets(F);
            if (use_json) {

            } else {
                os << "#V,#E,#F: " << V.cols() << "," << E.cols() << "," << F.cols() << std::endl;
            }
        }
        if (do_all || res["bounding_box"].as<bool>()) {
            auto bb = mtao::geometry::bounding_box(V);
            if (use_json) {

                const auto &m = bb.min();
                const auto &M = bb.max();
                // clang-format off
            js["bounding_box"] = {
                {"min",
                    {
                        { "x", m.x() },
                        { "y", m.y() },
                        { "z", m.z() }
                    }
                },
                {"max",
                    {
                        { "x", M.x() },
                        { "y", M.y() },
                        { "z", M.z() }
                    }
                }
            };
                // clang-format on
            } else {
                os << "Bounding box: " << bb.min().transpose() << " => " << bb.max().transpose() << std::endl;
            }
        }
        if (use_json) {
            os << js.dump(2) << std::endl;
        }
    }
}
