#pragma once
#include <array>
#include "mtao/eigen/stl2eigen.hpp"
#include "mtao/json/vector.hpp"
#include <Eigen/Geometry>
#include <nlohmann/json.hpp>


namespace nlohmann {
template<typename Scalar, int D>
struct adl_serializer<Eigen::AlignedBox<Scalar, D>> {
    static void to_json(json &j, const Eigen::AlignedBox<Scalar, D> &ab) {
        j["min"] = ab.min();
        j["max"] = ab.max();
    }


    // expect either an array or "x": something, "y": something
    static void from_json(const json &j, Eigen::AlignedBox<Scalar, D> &ab) {

        mtao::json::detail::json2vector(j["min"], ab.min());
        mtao::json::detail::json2vector(j["max"], ab.max());
    }
};
}// namespace nlohmann


namespace mtao::json {
template<typename Scalar, int D>
nlohmann::json bounding_box2json(const Eigen::AlignedBox<Scalar, D> &bbox) {
    nlohmann::json obj;
    for (int j = 0; j < D; ++j) {
        obj["min"][std::string(1, 'x' + j)] = bbox.min()(j);
        obj["max"][std::string(1, 'x' + j)] = bbox.max()(j);
    }
    return obj;
}

}// namespace mtao::json
