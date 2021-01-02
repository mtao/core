#include "mtao/geometry/strands.h"
#include <vector>
#include <iostream>
#include <algorithm>


int main() {
    using namespace mtao::geometry;
    std::vector<strand> frags({ strand(0, 1),
                                strand(3, 2),
                                strand(2, 1),
                                strand(4, 0),
                                strand(3, 4) });


    if (auto je = frags[0].joint_ends(frags[1])) {
        std::cout << "joint ends: ";
        for (auto &&j : *je) {
            std::cout << j << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "invalid join" << std::endl;
    }
    if (auto je = frags[0].joint_ends(frags[2])) {
        std::cout << "joint ends: ";
        for (auto &&j : *je) {
            std::cout << j << " ";
        }
        std::cout << std::endl;
        std::cout << "frags0: ";
        frags[0].merge(frags[2]);
        for (auto &&j : frags[0].data) {
            std::cout << j << " ";
        }
        std::cout << std::endl;
        std::cout << "frags2: ";
        for (auto &&j : frags[2].data) {
            std::cout << j << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "invalid join" << std::endl;
    }


    auto frag = mtao::geometry::merge(frags);
    for (auto &&j : frag.data) {
        std::cout << j << " ";
    }
    std::cout << std::endl;
}
