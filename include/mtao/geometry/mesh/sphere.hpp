#pragma once

//This file is ehre for historical reasons
#include "shapes/sphere.hpp"


namespace mtao::geometry::mesh {
template<typename T, int D = 3>
std::tuple<mtao::ColVectors<T, D>, mtao::ColVectors<int, D>> sphere(int depth) {
    return shapes::sphere<T, D>(depth);
}
}// namespace mtao::geometry::mesh
