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

    constexpr static bool StaticRow = Mat::RowsAtCompileTime != Eigen::Dynamic;
    constexpr static bool StaticCol = Mat::ColsAtCompileTime != Eigen::Dynamic;
    constexpr static bool RowVec = Mat::RowsAtCompileTime == 1;
    constexpr static bool ColVec = Mat::ColsAtCompileTime == 1;
    constexpr static bool IsVec = RowVec || ColVec;
    using Scalar = typename Mat::Scalar;
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


    static bool try_misc_vec_formats(const json &js, Mat &M) {
        if constexpr (!IsVec) {
            return false;
        }
        if (js.is_array()) {

            if constexpr (RowVec) {
                M = mtao::eigen::stl2eigen(js.get<std::vector<Scalar>>());
                return true;
            } else if constexpr (ColVec) {
                M = mtao::eigen::stl2eigen(js.get<std::vector<Scalar>>()).transpose();
                return true;
            }
        } else if (js.contains("x")) {

            int count;
            if constexpr (StaticRow && StaticCol) {
                count = Mat::RowsAtCompileTime * Mat::ColsAtCompileTime;
            } else {
                count = js.size();
                M.resize(count);
            }
            std::string chr = "x";
            for (int j = 0; j < count; ++j) {

                chr[0] = 'x' + j;
                M(j) = js[chr];
            }
            return true;
        }
        return false;
    }

    static void from_json(const json &js, Mat &M) {
        static_assert(mtao::eigen::concepts::PlainObjectBaseDerived<Mat>);

        int rows = js.contains("rows") ? js["rows"].get<int>() : 1;
        int cols = js.contains("cols") ? js["cols"].get<int>() : 1;
        if constexpr (StaticRow) {
            rows = Mat::RowsAtCompileTime;
        }
        if constexpr (StaticCol) {
            cols = Mat::ColsAtCompileTime;
        }
        M.resize(rows, cols);
        if constexpr (RowVec || ColVec) {
            if (try_misc_vec_formats(js, M)) {
                return;
            }
        }
        const auto &data = js["data"];
        if constexpr (RowVec) {
            M = mtao::eigen::stl2eigen(data.template get<std::vector<typename Mat::Scalar>>()).transpose();
            return;
        }
        if constexpr (ColVec) {
            M = mtao::eigen::stl2eigen(data.template get<std::vector<typename Mat::Scalar>>());
            return;
        }
        for (auto &&[row, v] : mtao::iterator::enumerate(data)) {
            auto r = M.row(row);

            r = mtao::eigen::stl2eigen(v.template get<std::vector<typename Mat::Scalar>>()).transpose();
        }
    }
};


}// namespace nlohmann
