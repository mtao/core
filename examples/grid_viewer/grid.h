#pragma once
#include <mtao/types.hpp>


mtao::ColVectors<unsigned int,2> make_edge_topology(const int i, const int j, const int k);
mtao::ColVectors<unsigned int,3> make_topology(const int i, const int j, const int k);
mtao::ColVectors<float,3> make_vertices(const int i, const int j, const int k);
std::tuple<mtao::ColVectors<float,3>,mtao::ColVectors<unsigned int,3>> make_mesh(const int i, const int j, const int k);

