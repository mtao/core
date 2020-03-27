#ifndef READ_OBJ_H
#define READ_OBJ_H

#include <tuple>

#include "mtao/types.h"

namespace mtao::geometry::mesh {

// simple OBJ loader, supports quads
std::tuple<mtao::ColVectors<float, 3>, mtao::ColVectors<int, 3>> read_objF(
    const std::string& filename);
std::tuple<mtao::ColVectors<double, 3>, mtao::ColVectors<int, 3>> read_objD(
    const std::string& filename);

// 2d variant of obj using e instead of f
std::tuple<mtao::ColVectors<float, 2>, mtao::ColVectors<int, 2>> read_obj2F(
    const std::string& filename);
std::tuple<mtao::ColVectors<double, 2>, mtao::ColVectors<int, 2>> read_obj2D(
    const std::string& filename);

//  ==============================================
// read a buffer directly, more for testing
std::tuple<mtao::ColVectors<float, 3>, mtao::ColVectors<int, 3>> read_objF(
    std::istream& filename);
std::tuple<mtao::ColVectors<double, 3>, mtao::ColVectors<int, 3>> read_objD(
    std::istream& filename);

// 2d variant of obj using e instead of f
std::tuple<mtao::ColVectors<float, 2>, mtao::ColVectors<int, 2>> read_obj2F(
    std::istream& filename);
std::tuple<mtao::ColVectors<double, 2>, mtao::ColVectors<int, 2>> read_obj2D(
    std::istream& filename);
}  // namespace mtao::geometry::mesh

#endif  // READ_OBJ_H
