#pragma once
#include <Eigen/Geometry>
#include "mtao/types.hpp"
#include "mtao/colvector_loop.hpp"


namespace mtao {
namespace geometry {
    template<typename T, int D>
    using BBox = Eigen::AlignedBox<T, D>;
    template<typename T, int D>
    auto bounding_box(const mtao::ColVectors<T, D> &V) {
        BBox<T, D> bb;
        if (V.cols() > 0) {
            bb.extend(V.rowwise().minCoeff());
            bb.extend(V.rowwise().maxCoeff());
        }
        return bb;
    }
    template<typename T, int D>
    auto bounding_box_slow(const mtao::ColVectors<T, D> &V) {
        BBox<T, D> bb;
        if (V.cols() > 0) {
            for (auto &&p : colvector_loop(V)) {
                bb.extend(p);
            }
        }

        return bb;
    }

    //expand by multiplying by a scale
    template<typename T, int D>
    BBox<T, D> expand_bbox(BBox<T, D> bb, T scale) {
        using Vec = mtao::Vector<T, D>;
        Vec s = (scale - 1) * (T(.5) * bb.sizes());
        bb.min() -= s;
        bb.max() += s;
        return bb;
    }
    //expand by adding a factor
    template<typename T, int D>
    BBox<T, D> offset_bbox(BBox<T, D> bb, T offset) {
        bb.min().array() -= offset;
        bb.max().array() += offset;
        return bb;
    }
}// namespace geometry
}// namespace mtao
