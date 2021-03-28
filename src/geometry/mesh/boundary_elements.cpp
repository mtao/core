#include "mtao/geometry/mesh/boundary_elements.h"

namespace mtao::geometry::mesh {

// Given a boundary_index->sign representation of a mesh compute the cell boundaries
std::set<int> boundary_element_indices(const std::vector<std::map<int, bool>> &cells) {
    std::map<int, char> jumps;

    for (auto &&cell : cells) {
        for (auto &&[face_index, sign] : cell) {
            jumps[face_index] |= sign ? 1 : 2;
        }
    }
    std::set<int> ret;
    for (auto &&[face_index, jump] : jumps) {
        if (jump == 2 || jump == 1) {
            ret.emplace(face_index);
        }
    }
    return ret;
}
}// namespace mtao::geometry::mesh
