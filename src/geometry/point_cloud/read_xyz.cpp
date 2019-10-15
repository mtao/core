#include "mtao/geometry/point_cloud/read_xyz.hpp"
#include "mtao/eigen/stl2eigen.hpp"
#include <fstream>
#include <iterator>
#include <array>

namespace mtao::geometry::point_cloud {
    template <typename T>
        std::array<mtao::ColVectors<T,3>,2> read_xyz(const std::string& filename) {
            std::vector<std::array<T,6>> lines;

            int min_linelen = std::numeric_limits<int>::max();

            std::ifstream ifs(filename);

            for(std::string line; std::getline(ifs,line);) {
                std::istringstream iss(line);
                std::vector<std::string> tokens;
                std::copy(std::istream_iterator<std::string>(iss)
                        , std::istream_iterator<std::string>()
                        , std::back_insert_iterator<std::vector<std::string> >(tokens));

                int linelen = std::min<int>(tokens.size(),6);
                min_linelen = std::min(linelen,min_linelen);
                std::array<T,6> V;
                std::fill(V.begin(),V.end(),0);
                std::transform(tokens.begin(),tokens.begin()+linelen, V.begin(), [&](const std::string& s) {
                        return std::stof(s);
                        });
                lines.emplace_back(V);

            }
            mtao::MatrixX<T> D = mtao::eigen::stl2eigen(lines);
            if(min_linelen <= 3) {
                return {{D.topRows(3),{}}};
            } else {
                return {{D.topRows(3),D.bottomRows(3)}};
            }
        }

    std::array<mtao::ColVectors<float,3>,2> read_xyzF(const std::string& filename) {
        return read_xyz<float>(filename);
    }
    std::array<mtao::ColVectors<double,3>,2> read_xyzD(const std::string& filename) {
        return read_xyz<double>(filename);
    }
}
