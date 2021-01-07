#include <iostream>
#include <fstream>
#include <mtao/utils/corrade_configuration_json_interchange.hpp>
#include <cxxopts.hpp>


int main(int argc, char *argv[]) {


    cxxopts::Options options("json_to_corrade_configuration", "Converts a json file to a Corrade::Utility::Configuration file");
    // clang-format off
    options.add_options()
        ("input", "output json file", cxxopts::value<std::string>())
        ("output", "output Corrade::Utility::Configuration file", cxxopts::value<std::string>());
    // clang-format on

    options.parse_positional({ "input", "output" });
    auto res = options.parse(argc, argv);

    std::ifstream ifs(res["input"].as<std::string>());
    nlohmann::json js;
    ifs >> js;

    auto config = mtao::utils::json_to_corrade_configuration(js);


    config.save(res["output"].as<std::string>());
    return 0;
}

