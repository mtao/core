#pragma once
#include <mtao/types.h>


mtao::ColVectors<unsigned int,3> make_topology(const int i, const int j);
mtao::ColVectors<float,2> make_vertices(const int i, const int j);
std::tuple<mtao::ColVectors<float,2>,mtao::ColVectors<unsigned int,3>> make_mesh(const int i, const int j);

