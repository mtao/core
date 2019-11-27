#ifndef READ_XYZ_H
#define READ_XYZ_H

#include "mtao/types.hpp"
#include <array>

namespace mtao { namespace geometry { namespace point_cloud {

    //Read an xyz file assuming that it returns points and maybe 3 more values,
    //which are returned in two arrays
    //If any vertices lack normals the second array is empty
    std::array<mtao::ColVectors<float,3>,2> read_xyzF(const std::string& filename);
    std::array<mtao::ColVectors<double,3>,2> read_xyzD(const std::string& filename);

}}}


#endif//READ_XYZ_H
