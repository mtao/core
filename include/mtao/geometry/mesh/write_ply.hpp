#ifndef WRITE_PLY_H
#define WRITE_PLY_H

#include "mtao/types.hpp"
#include <tuple>

namespace mtao::geometry::mesh {

    template <typename T>
    write_ply(const mtao::ColVectors<T,3>& V, const mtao::ColVectors<T,3>& C,const mtao::ColVectors<int,3>& F, const std::string& filename);

    write_plyF(const mtao::ColVectors<float,3>& V,const mtao::ColVectors<int,3>& F, const std::string& filename);
    write_plyD(const mtao::ColVectors<double,3>& V,const mtao::ColVectors<int,3>& F, const std::string& filename);

    write_plyF(const mtao::ColVectors<float,3>& V, const mtao::ColVectors<float,3>& C, const mtao::ColVectors<int,3>& F, const std::string& filename);
    write_plyD(const mtao::ColVectors<double,3>& V, const mtao::ColVectors<double,3>& C,const mtao::ColVectors<int,3>& F, const std::string& filename);
}

#endif//READ_PLY_H
