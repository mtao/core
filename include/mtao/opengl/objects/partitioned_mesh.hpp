#pragma once
#include <Magnum/GL/MeshView.h>
#include <spdlog/spdlog.h>

#include <Eigen/Geometry>

#include "mtao/iterator/enumerate.hpp"
#include "mtao/opengl/objects/mesh.h"

namespace mtao::opengl::objects {
using namespace Magnum;
template <int D>
class PartitionedMesh : public Mesh<D> {
   public:
    using Mesh<D>::Mesh;
    void set_offsets(const std::vector<int>& offsets) {
        face_offsets = offsets;
    }

    Magnum::GL::MeshView get(size_t index) const;
    Corrade::Containers::Array<Magnum::GL::MeshView> views();

    std::vector<int> face_offsets = std::vector<int>{
        0, 0};  // if the start/end is 0,0 then Magnum draws everything
};

template <int D>
Magnum::GL::MeshView PartitionedMesh<D>::get(size_t index) const {
    Magnum::GL::MeshView view(*this);

    auto start = face_offsets.at(index);
    auto end = face_offsets.at(index + 1);
    view.setIndexRange(start).setCount(end - start);
    return view;
}

template <int D>
Corrade::Containers::Array<Magnum::GL::MeshView> PartitionedMesh<D>::views() {
    Corrade::Containers::Array<Magnum::GL::MeshView> views(
        Corrade::Containers::DirectInit, face_offsets.size() - 1, *this);

    for (auto&& [index, view] : mtao::iterator::enumerate(views)) {
        auto start = face_offsets.at(index);
        auto end = face_offsets.at(index + 1);
        view.setIndexRange(start).setCount(end - start);
        view.setIndexRange(0).setCount(0);
    }
    return views;
}
}  // namespace mtao::opengl::objects
