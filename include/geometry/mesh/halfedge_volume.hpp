#pragma once
#include "types.h"
#include "halfedge.hpp"


namespace mtao { namespace geometry { namespace mesh {

    template <typename T, int D>
        T volume(const mtao::ColVectors<T,D>& V, const HalfEdgeMesh& hem, int cell);
    template <typename T, int D>
        T dual_volume(const mtao::ColVectors<T,D>& V, const HalfEdgeMesh& hem, int vertex);


    template <typename T>
        T volume(const mtao::ColVectors<T,D>& V, const std::vector<int>& c) {
            auto o = V.col(c.back());

            auto tri_area = [&](int i, int j) {
                auto u = V.col(i)-o;
                auto v = V.col(j)-o;
                return .5 * u.cross(v).norm();
            };
            T ret = 0;
            for(int i = 0; i < c.size()-2; ++i) {
                ret += tri_area(i,i+1);
            }
            return ret;
        }

    template <typename T, int D>
        T volume(const mtao::ColVectors<T,D>& V, const HalfEdgeMesh& hem,int cell) {
            return volume(V,hem.cell(cell));
        }
    template <typename T, int D>
        T dual_volume(const mtao::ColVectors<T,D>& V, const HalfEdgeMesh& hem,int vertex) {
            return volume(V,hem.dual_cell(vertex));
        }
}}}
