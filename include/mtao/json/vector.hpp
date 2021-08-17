
#pragma once
#include "mtao/json/matrix.hpp"


namespace mtao::json {
template<typename Scalar, int D>
Vector<Scalar, D> json2vector(const nlohmann::json &js) {
    Eigen::Vector<Scalar, D> r;
    nlohmann::adl_serializer<Eigen::Vector<Scalar, D>>::from_json(js, r);
    return r;
}
}// namespace mtao::json
