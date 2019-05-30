#ifndef READ_OBJ_H
#define READ_OBJ_H

#include "mtao/types.h"
#include <tuple>

namespace mtao { namespace geometry { namespace mesh {

    std::tuple<mtao::ColVectors<float,3>,mtao::ColVectors<int,3>> read_objF(const std::string& filename);
    std::tuple<mtao::ColVectors<double,3>,mtao::ColVectors<int,3>> read_objD(const std::string& filename);

}}}


#endif//READ_OBJ_H
