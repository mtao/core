#pragma once
#include "mtao/types.h"
#include <tuple>

namespace mtao {
namespace geometry {
    namespace mesh {

        template<int D>
        using IndexType = mtao::ColVectors<int, D>;
        using MarkerType = mtao::VectorX<int>;

        std::tuple<mtao::ColVectors<float, 3>, MarkerType> read_nodeF(const std::string &filename);
        std::tuple<mtao::ColVectors<double, 3>, MarkerType> read_nodeD(const std::string &filename);

        std::tuple<IndexType<2>, MarkerType> read_edge(const std::string &filename);
        std::tuple<IndexType<3>, MarkerType> read_face(const std::string &filename);
        //ignore quadratic meshes
        std::tuple<IndexType<4>, MarkerType> read_ele(const std::string &filename);

    }// namespace mesh
}// namespace geometry
}// namespace mtao
