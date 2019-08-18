#pragma once
#include "mtao/opengl/objects/mesh.h"
#include <Eigen/Geometry>
#include "mtao/geometry/bounding_box_mesh.hpp"




namespace mtao::opengl::objects {
    using namespace Magnum;
    template <int D>
    class BoundingBox: public Mesh<D> {
        public:
        BoundingBox(): BoundingBox(Eigen::AlignedBox<float,D>(mtao::Vector<float,D>::Zero(), mtao::Vector<float,D>::Ones())) {}
        BoundingBox(const Eigen::AlignedBox<float,D>& bb) {
            set_bbox(bb);
        }
        void set_bbox(const Eigen::AlignedBox<float,D>& bb) {
            if constexpr(D == 2) {
                auto [V,E] = mtao::geometry::bounding_box_mesh(bb);
                Mesh<D>::setVertexBuffer(V);
                Mesh<D>::setEdgeBuffer(E.template cast<unsigned int>());
            } else {
                auto [V,F,E] = mtao::geometry::bounding_box_mesh<true>(bb);
                Mesh<D>::setVertexBuffer(V);
                Mesh<D>::setEdgeBuffer(E.template cast<unsigned int>());
                Mesh<D>::setTriangleBuffer(F.template cast<unsigned int>());
            }
        }

    };
}
