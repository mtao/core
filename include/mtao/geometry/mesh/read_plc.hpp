#pragma once
#include <tuple>
#include "mtao/geometry/mesh/plc.hpp"

namespace mtao { namespace geometry { namespace mesh {

    template <int D>
        std::tuple<mtao::Vector<int,D>, int> read_elem_line(const std::string& s) {
            std::stringstream ss(s);

            mtao::Vector<int,D> v;
            int marker = 0;
            for(int i = 0; ss && i < D; ++i) {
                ss >> v(i);
            }
            if(ss) {
                ss >> marker;
            }
            return {v,marker};
        }

    template <typename T, int D, typename T2, int D2>
        std::tuple<int, mtao::Vector<T,D>, mtao::Vector<T,D>, int> read_vec_line(const std::string& s) {
            std::istringstream ss(s);

            mtao::Vector<T,D> v;
            mtao::Vector<T2,D2> v;

            int index;
            ss >> index;
            int marker = 0;
            for(int i = 0; ss && i < D; ++i) {
                ss >> v(i);
            }
            if(ss) {
                ss >> marker;
            }
            return {index,v,a,marker};
        }

    template <typename T, int D>
        std::tuple<mtao::ColVectors<T,D>,mtao::VectorX<int>> read_file(const std::string& filename) {

            std::ifstream ifs(filename);

            std::string line; std::getline(ifs,line);

            int size;
            std::istringstream ss(line);

            ss >> size;

            mtao::ColVectors<T,D> V(D,size);
            mtao::VectorX<int> M(size);

            if constexpr(std::is_same_v<T,int>) {
                int i = 0;
                for(std::string line; std::getline(ifs,line);++i) {
                    auto [v,m] = read_elem_line<D>(line);
                    V.col(i) = v;
                    M(i) = m;

                }
            } else {
                for(std::string line; std::getline(ifs,line);) {
                    auto [i,v,m] = read_vec_line<T,D>(line);
                    V.col(i) = v;
                    M(i) = m;
                }
            }

            return {V,M};

        }

    template <typename T, int D, typename T2 = int, int D2 = 0>
    std::tuple<mtao::ColVectors<T,D>, mtao::ColVectors<T2,D2>,plc::MarkerType> read_node(const std::string& filename) {
        return read_file<T,D,T2,D2>(filename);
    }

    template <typename T2 = int, int D2 = 0>
    std::tuple<plc::IndexType<2>,mtao::ColVectors<T2,D2>,plc::MarkerType> read_edge(const std::string& filename) {
        return read_file<int,2,T2,D2>(filename);
    }
    template <typename T2 = int, int D2 = 0>
    std::tuple<plc::IndexType<3>,mtao::ColVectors<T2,D2>,plc::MarkerType> read_face(const std::string& filename);
        return read_file<int,3,T2,D2>(filename);
    }
    //ignore quadratic meshes
    template <typename T2 = int, int D2 = 0>
    std::tuple<plc::IndexType<4>,mtao::ColVectors<T2,D2>,plc::MarkerType> read_ele(const std::string& filename);
        return read_file<int,4,T2,D2>(filename);
    }

}}}


