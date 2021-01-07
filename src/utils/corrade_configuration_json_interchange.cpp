
#include "mtao/utils//corrade_configuration_json_interchange.hpp"
#include <spdlog/spdlog.h>
#include <iostream>


namespace mtao::utils {

namespace {
    void configuration_to_json(const Corrade::Utility::ConfigurationGroup &config, auto &&js, bool support_multiple_values) {

        for (auto &&key : config.valueKeys()) {
            // for some resaon i'm getting empty keys
            if (key.empty()) continue;
            //spdlog::info("value {} = {}", key, fmt::join(config.values(key), ","));

            if (support_multiple_values) {
                js[key] = config.values(key);
            } else {
                js[key] = config.value(key);
            }
        }

        for (auto &&groupName : config.groupNames()) {


            //spdlog::info("groupName: {}", groupName);
            auto groups = config.groups(groupName);

            for (auto &&group_ptr : groups) {

                if (js.contains(groupName)) {
                    auto &subjs = js[groupName] = nlohmann::json::object();
                    configuration_to_json(*group_ptr, subjs, support_multiple_values);
                } else {
                    auto &subjs = js[groupName];
                    configuration_to_json(*group_ptr, subjs, support_multiple_values);
                }
            }
        }
    }

    void json_to_configuration(const auto &js, Corrade::Utility::ConfigurationGroup &config) {

        for (auto &&[k, v] : js.items()) {
            if (v.is_object()) {
                json_to_configuration(v, *config.addGroup(k));
            } else if (v.is_array()) {
                for (auto &&v : v) {
                    config.addValue(k, std::string(v));
                }
            } else {
                config.setValue(k, std::string(v));
            }
        }
    }

}// namespace
nlohmann::json corrade_configuration_to_json(const Corrade::Utility::Configuration &config, bool support_multiple_values) {
    using json = nlohmann::json;
    json js;
    configuration_to_json(config, js, support_multiple_values);
    return js;
}
Corrade::Utility::Configuration json_to_corrade_configuration(const nlohmann::json &json, Corrade::Utility::Configuration::Flags flags) {
    Corrade::Utility::Configuration config(flags);


    json_to_configuration(json, config);


    return config;
}

}// namespace mtao::utils

