#pragma once
#include <Eigen/Geometry>
#include "types.h"
#include "colvector_loop.hpp"


namespace mtao { namespace geometry {
    template <typename T, int D>
        auto bounding_box(const mtao::ColVectors<T,D>& V) {
            Eigen::AlignedBox<T,D> bb;
            bb.extend(V.rowwise().minCoeff());
            bb.extend(V.rowwise().maxCoeff());
            return bb;
        }
    template <typename T, int D>
        auto bounding_box_slow(const mtao::ColVectors<T,D>& V) {
            Eigen::AlignedBox<T,D> bb;
            bb.extend(V.rowwise().minCoeff());
            bb.extend(V.rowwise().maxCoeff());
            for(auto&& p: colvector_loop(V)) {
                bb.extend(p);
            }

            return bb;
        }
}}
