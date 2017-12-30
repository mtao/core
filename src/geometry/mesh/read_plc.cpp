#include <array>
#include "mtao/geometry/mesh/read_plc.hpp"
#include <fstream>
#include <iterator>
#include "mtao/logging/logger.hpp"

using namespace mtao::logging;

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

    template <typename T, int D>
        std::tuple<int, mtao::Vector<T,D>, int> read_vec_line(const std::string& s) {
            std::istringstream ss(s);

            mtao::Vector<T,D> v;

            int index;
            ss >> index;
            int marker = 0;
            for(int i = 0; ss && i < D; ++i) {
                ss >> v(i);
            }
            if(ss) {
                ss >> marker;
            }
            return {index,v,marker};
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


std::tuple<mtao::ColVectors<float,3>,MarkerType> read_nodeF(const std::string& filename) {return read_file<float,3>(filename);}
std::tuple<mtao::ColVectors<double,3>,MarkerType> read_nodeD(const std::string& filename) {return read_file<double,3>(filename);}
std::tuple<IndexType<2>, MarkerType> read_edge(const std::string& filename) {return read_file<int,2>(filename);}
std::tuple<IndexType<3>,MarkerType> read_face(const std::string& filename) {return read_file<int,3>(filename);}
std::tuple<IndexType<4>,MarkerType> read_ele(const std::string& filename) {return read_file<int,4>(filename);}
}}}
