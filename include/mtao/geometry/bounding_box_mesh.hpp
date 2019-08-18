#pragma once
#include <Eigen/Geometry>
#include "mtao/types.hpp"

namespace mtao::geometry {
    template <bool OutputEdges=false, typename T=double, int D=3>
        auto  bounding_box_mesh(const Eigen::AlignedBox<T,D>& bb) {
            mtao::ColVectors<T,D> V(D,1<<D);
            if constexpr(D == 2) {
                mtao::ColVectors<int,2> E(2,4);
                V << bb.corner(Eigen::AlignedBox<T,D>::CornerType::BottomLeft),
                  bb.corner(Eigen::AlignedBox<T,D>::CornerType::BottomRight),
                  bb.corner(Eigen::AlignedBox<T,D>::CornerType::TopLeft),
                  bb.corner(Eigen::AlignedBox<T,D>::CornerType::TopRight);
                E << 0,1,3,2,
                  1,2,0,3;
                return std::make_tuple(V,E);
            } else {//D == 3
                mtao::ColVectors<int,3> F(3,12);
                V << 
                    bb.corner(Eigen::AlignedBox<T,D>::CornerType::BottomLeftFloor),
                    bb.corner(Eigen::AlignedBox<T,D>::CornerType::BottomRightFloor),
                    bb.corner(Eigen::AlignedBox<T,D>::CornerType::TopLeftFloor),
                    bb.corner(Eigen::AlignedBox<T,D>::CornerType::TopRightFloor),
                    bb.corner(Eigen::AlignedBox<T,D>::CornerType::BottomLeftCeil),
                    bb.corner(Eigen::AlignedBox<T,D>::CornerType::BottomRightCeil),
                    bb.corner(Eigen::AlignedBox<T,D>::CornerType::TopLeftCeil),
                    bb.corner(Eigen::AlignedBox<T,D>::CornerType::TopRightCeil);
                //0:000
                //1:010
                //2:100
                //3:110
                //4:001
                //5:011
                //6:101
                //7:111
                F <<
                    6,2,3,1,7,3,6,7,4,5,5,7,
                    0,0,0,0,2,2,4,4,0,0,1,1,
                    4,6,2,3,6,7,7,5,5,1,7,3;

                if constexpr(OutputEdges) {
                    mtao::ColVectors<int,2> E(2,12);
                    E << 0,0,0,1,1,2,2,3,4,4,5,6,
                      1,2,4,3,5,3,6,7,5,6,7,7;
                    return std::make_tuple(V,F,E);
                } else {
                    return std::make_tuple(V,F);
                }
            }
        }
}
