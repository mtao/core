#include "mtao/geometry/mesh/shapes/cube.hpp"
#include <iostream>


namespace mtao::geometry::mesh::shapes {
    namespace cube_internal {
        // constexpr static T _V[36] = {
        //     T(0), T(1), T(1), T(0), T(0), T(1), T(1), T(0),
        //     T(0), T(0), T(1), T(1), T(1), T(1), T(0), T(0),
        //     T(0), T(0), T(0), T(0), T(1), T(1), T(1), T(1)};


            //T(0), T(0), T(0),
            //T(1), T(0), T(0),
            //T(1), T(1), T(0),
            //T(0), T(1), T(0),
            //T(0), T(1), T(1),
            //T(1), T(1), T(1),
            //T(1), T(0), T(1),
            //T(0), T(0), T(1)};
        template <>
            const float vert_container<float>::_V[36] = {
            float(0), float(0), float(0),
            float(1), float(0), float(0),
            float(1), float(1), float(0),
            float(0), float(1), float(0),
            float(0), float(1), float(1),
            float(1), float(1), float(1),
            float(1), float(0), float(1),
            float(0), float(0), float(1)};
        template <>
            const double vert_container<double>::_V[36] = {
            double(0), double(0), double(0),
            double(1), double(0), double(0),
            double(1), double(1), double(0),
            double(0), double(1), double(0),
            double(0), double(1), double(1),
            double(1), double(1), double(1),
            double(1), double(0), double(1),
            double(0), double(0), double(1)};


            const int _F[36] = { 0, 2, 1, //face front
              0, 3, 2,
              2, 3, 4, //face top
              2, 4, 5,
              1, 2, 5, //face right
              1, 5, 6,
              0, 7, 4, //face left
              0, 4, 3,
              5, 4, 7, //face back
              5, 7, 6,
              0, 6, 7, //face bottom
              0, 1, 6};
    }
}
