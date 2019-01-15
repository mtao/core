#pragma once
#include <mtao/types.h>


inline int index(const int i, const int j, int ii, int jj) {
    return ii + i * jj;
}
mtao::ColVectors<unsigned int,3> make_topology(const int i, const int j);
mtao::ColVectors<float,2> make_vertices(const int i, const int j);
std::tuple<mtao::ColVectors<float,2>,mtao::ColVectors<unsigned int,3>> make_mesh(const int i, const int j);

