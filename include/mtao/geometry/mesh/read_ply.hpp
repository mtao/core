#ifndef READ_PLY_H
#define READ_PLY_H

#include "mtao/types.hpp"
#include <tuple>

namespace mtao::geometry::mesh {

std::tuple<mtao::ColVectors<float, 3>, mtao::ColVectors<int, 3>> read_plyF(const std::string &filename);
std::tuple<mtao::ColVectors<double, 3>, mtao::ColVectors<int, 3>> read_plyD(const std::string &filename);

}// namespace mtao::geometry::mesh


#endif//READ_PLY_H
