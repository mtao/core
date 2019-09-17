#ifndef READ_XYZ_H
#define READ_XYZ_H

#include "mtao/types.hpp"
#include <array>

namespace mtao { namespace geometry { namespace point_cloud {

    std::array<mtao::ColVectors<float,3>,2> read_xyzF(const std::string& filename);
    std::array<mtao::ColVectors<double,3>,2> read_xyzD(const std::string& filename);

}}}


#endif//READ_OBJ_H
