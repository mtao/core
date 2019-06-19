#pragma once
#include "mtao/opengl/objects/mesh.h"
#include <Eigen/Geometry>




namespace mtao::opengl::objects {
    using namespace Magnum;
    template <int D>
    class BoundingBox: public Mesh<D> {
        public:
        BoundingBox(): BoundingBox(Eigen::AlignedBox<float,D>(mtao::Vector<float,D>::Zero(), mtao::Vector<float,D>::Ones())) {}
        BoundingBox(const Eigen::AlignedBox<float,D>& bb) {
                mtao::ColVectors<float,D> V(2,1<<D);
            if constexpr(D == 2) {
                V << bb.corner(Eigen::AlignedBox<float,D>::CornerType::BottomLeft),
                  bb.corner(Eigen::AlignedBox<float,D>::CornerType::BottomRight),
                  bb.corner(Eigen::AlignedBox<float,D>::CornerType::TopLeft),
                  bb.corner(Eigen::AlignedBox<float,D>::CornerType::TopRight);
            } else {
                V << 
                    bb.corner(Eigen::AlignedBox<float,D>::CornerType::BottomLeftFloor),
                    bb.corner(Eigen::AlignedBox<float,D>::CornerType::BottomRightFloor),
                    bb.corner(Eigen::AlignedBox<float,D>::CornerType::TopLeftFloor),
                    bb.corner(Eigen::AlignedBox<float,D>::CornerType::TopRightFloor),
                    bb.corner(Eigen::AlignedBox<float,D>::CornerType::BottomLeftCeil),
                    bb.corner(Eigen::AlignedBox<float,D>::CornerType::BottomRightCeil),
                    bb.corner(Eigen::AlignedBox<float,D>::CornerType::TopLeftCeil),
                    bb.corner(Eigen::AlignedBox<float,D>::CornerType::TopRightCeil);
            }
            Mesh<D>::setVertexBuffer(V);
            GL::Mesh::setCount(0);
            GL::Mesh::setPrimitive(Magnum::MeshPrimitive::Points);


        }

    };
}
