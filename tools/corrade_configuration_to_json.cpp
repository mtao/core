#include <iostream>
#include <iomanip>
#include <fstream>
#include <mtao/utils/corrade_configuration_json_interchange.hpp>
#include <cxxopts.hpp>


int main(int argc, char *argv[]) {


    cxxopts::Options options("corrade_configuration_to_json", "Converts a Corrade::Utility::Configuration file to json");
    // clang-format off
    options.add_options()
        ("input", "output Corrade::Utility::Configuration file", cxxopts::value<std::string>())
        ("output", "output json file", cxxopts::value<std::string>())
        ("m,multiple", "allow for multiple files", cxxopts::value<bool>()->default_value("false"));

    // clang-format on

    options.parse_positional({ "input", "output" });
    auto res = options.parse(argc, argv);

    Corrade::Utility::Configuration config{ res["input"].as<std::string>() };

    auto js = mtao::utils::corrade_configuration_to_json(config, res["multiple"].as<bool>());

    std::ofstream ofs(res["output"].as<std::string>());
    ofs << std::setw(2) << js << std::endl;


    return 0;
}

