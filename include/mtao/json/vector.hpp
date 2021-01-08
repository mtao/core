#pragma once
#include "mtao/eigen/stl2eigen.hpp"
#include "mtao/types.hpp"
#include "mtao/iterator/zip.hpp"
#include "mtao/eigen/iterable.hpp"
#include <nlohmann/json.hpp>
#include <string>

/*
namespace nlohmann {
template<typename Derived>
struct adl_serializer<Eigen::MatrixBase<Derived>> {
    static void to_json(json &j, const Eigen::MatrixBase<Derived> &M) {

        j["rows"] = M.rows();
        j["cols"] = M.cols();

        // data is always stored col major
        auto &data = j["data"] = nlohmann::json::object();
        for (int r = 0; r < M.rows(); ++r) {
            auto &row = data[std::to_string(r)] = json::array({});
            row.reserve(M.cols());
            for (int c = 0; c < M.cols(); ++c) {
                row.emplace_back(M(c));
            }

        }
    }
};

template<typename Derived>
struct adl_serializer<Eigen::PlainObjectBase<Derived>> {
    static void from_json(const json &js, Eigen::PlainObjectBase<Derived> &M) {
        int rows = js.contains("rows") ? js["rows"].get<int>() : 1;
        int cols = js.contains("cols ") ? js["cols"].get<int>() : 1;

        M.resize(rows, cols);
        for (auto &&[k, v] : js["data"]) {
            int row = std::stoi(k);
            auto r = M.row(row);

            r = mtao::eigen::stl2eigen(v.template get<std::vector<typename Derived::Scalar>>());
        }
    }
};


template<typename Scalar, int R, int C>
struct adl_serializer<Eigen::Matrix<Scalar, R, C>> {
    static void to_json(json &j, const Eigen::Matrix<Scalar, R, C> &M) {
    }
    static void from_json(const json &js, Eigen::Matrix<Scalar, R, C> &M) {
    }
};// namespace nlohmann


}// namespace nlohmann
*/


namespace mtao::json {

namespace detail {
    template<typename Derived>
    void json2vector(const nlohmann::json &js, Eigen::PlainObjectBase<Derived> &v) {

        using Scalar = typename Derived::Scalar;
        constexpr static int D = Derived::RowsAtCompileTime;
        static_assert(D != Eigen::Dynamic);
        if (js.is_array()) {
            v = mtao::eigen::stl2eigen(js.template get<std::array<Scalar, D>>());
        } else {
            for (auto &&[k, val] : js.items()) {
                if (k.size() != 1) { continue; }
                int r = k[0] - 'x';
                if (r < 0 || r >= Derived::RowsAtCompileTime) {
                    continue;
                }
                v(r) = val.template get<Scalar>();
            }
        }
    }
}// namespace detail

template<typename Scalar, int D>
Vector<Scalar, D> json2vector(const nlohmann::json &js) {
    Eigen::Vector<Scalar, D> r;
    detail::json2vector(js, r);
    return r;
}
}// namespace mtao::json
