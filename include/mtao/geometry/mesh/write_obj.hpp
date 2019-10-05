#ifndef WRITE_OBJ_H
#define WRITE_OBJ_H

#include "mtao/types.hpp"
#include <tuple>

namespace mtao::geometry::mesh {

    template <typename T>
    void write_obj(const mtao::ColVectors<T,3>& V,const mtao::ColVectors<int,3>& F, const std::string& filename);

    /*
    template <>
    extern void write_obj(const mtao::ColVectors<float,3>& V,const mtao::ColVectors<int,3>& F, const std::string& filename);
    template <>
    extern void write_obj(const mtao::ColVectors<double,3>& V,const mtao::ColVectors<int,3>& F, const std::string& filename);
    */

    void write_objF(const mtao::ColVectors<float,3>& V,const mtao::ColVectors<int,3>& F, const std::string& filename);
    void write_objD(const mtao::ColVectors<double,3>& V,const mtao::ColVectors<int,3>& F, const std::string& filename);

}

#endif//READ_OBJ_H
