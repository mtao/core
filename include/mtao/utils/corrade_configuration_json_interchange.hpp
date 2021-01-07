#pragma once
#include <Corrade/Utility/Configuration.h>

#include <nlohmann/json.hpp>


namespace mtao::utils {

// corrade config files are nicer to manually produce than json, but more annoying to parse / pass through subsequent pipelines. These two implement a basic way of transcoding between the interfaces
nlohmann::json corrade_configuration_to_json(const Corrade::Utility::Configuration &config, bool support_multiple_values = false);
Corrade::Utility::Configuration json_to_corrade_configuration(const nlohmann::json &json, Corrade::Utility::Configuration::Flags flags = {});

}// namespace mtao::utils

