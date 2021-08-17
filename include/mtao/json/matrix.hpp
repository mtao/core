#pragma once
#include "mtao/eigen/stl2eigen.hpp"
#include "mtao/types.hpp"
#include <iostream>
#include "mtao/iterator/zip.hpp"
#include "mtao/iterator/enumerate.hpp"
#include "mtao/eigen/iterable.hpp"
#include <nlohmann/json.hpp>
#include "mtao/eigen/concepts.hpp"
#include <string>

namespace nlohmann {
template<mtao::eigen::concepts::DenseBaseDerived Mat>
struct adl_serializer<Mat> {
    static void to_json(json &j, const Mat &M) {

        if (M.rows() != 1) {
            j["rows"] = M.rows();
        }
        if (M.cols() != 1) {
            j["cols"] = M.cols();
        }

        if (M.rows() == 1 || M.cols() == 1) {
            auto &data = j["data"] = nlohmann::json::array();
            for (int i = 0; i < M.size(); ++i) {
                data.emplace_back(M(i));
            }
        } else {
            // data is always stored col major
            auto &data = j["data"] = nlohmann::json::array();
            for (int r = 0; r < M.rows(); ++r) {
                auto &row = data.emplace_back(json::array({}));
                //row.reserve(M.cols());
                for (int c = 0; c < M.cols(); ++c) {
                    row.emplace_back(M(r, c));
                }
            }
        }
    }
    static void from_json(const json &js, Mat &M) {
        if constexpr (mtao::eigen::concepts::PlainObjectBaseDerived<Mat>) {
            int rows = js.contains("rows") ? js["rows"].get<int>() : 1;
            int cols = js.contains("cols") ? js["cols"].get<int>() : 1;
            M.resize(rows, cols);
            const auto &data = js["data"];
            if constexpr (Mat::RowsAtCompileTime == 1) {
                if (M.rows() == 1) {
                    M = mtao::eigen::stl2eigen(data.template get<std::vector<typename Mat::Scalar>>()).transpose();
                    return;
                }
            }
            if constexpr (Mat::ColsAtCompileTime == 1) {
                if (M.cols() == 1) {
                    M = mtao::eigen::stl2eigen(data.template get<std::vector<typename Mat::Scalar>>());
                    return;
                }
            }
            for (auto &&[row, v] : mtao::iterator::enumerate(data)) {
                auto r = M.row(row);

                r = mtao::eigen::stl2eigen(v.template get<std::vector<typename Mat::Scalar>>()).transpose();
            }
        } else {

            throw std::bad_cast("nhlomann json: cannot write to a Eigen object without storage");
        }
    }
};


}// namespace nlohmann
