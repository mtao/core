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

        ab.min() = j["min"].get<Eigen::Vector<Scalar, D>>();
        ab.max() = j["max"].get<Eigen::Vector<Scalar, D>>();
    }
};
}// namespace nlohmann


namespace mtao::json {
template<typename Scalar, int D>
nlohmann::json bounding_box2json(const Eigen::AlignedBox<Scalar, D> &bbox) {
    nlohmann::json js;
    nlohmann::adl_serializer<Eigen::AlignedBox<Scalar, D>>::to_json(js, bbox);
    return js;
}

}// namespace mtao::json
