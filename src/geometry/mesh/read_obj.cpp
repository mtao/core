#include <array>
#include "geometry/mesh/read_obj.hpp"
#include <fstream>
#include <iterator>
#include "logging/logger.hpp"

using namespace mtao::logging;

namespace mtao { namespace geometry { namespace mesh {

template <typename T>
struct MeshReader {

    using Vec = std::array<T,3>;
    using Tri = std::array<int,3>;
    using TokIt = std::vector<std::string>::const_iterator;



    static Vec process_vertex(TokIt begin, const TokIt& end) {
        Vec v = {{0,0}};
        std::transform(begin,end,v.begin(),[](const std::string& s) {
                return std::stof(s);});

        /*
           size_t dist = std::distance(begin,end);
           if(dist == 4) {
           float w = std::stof(*(begin+3));
           for(auto&& s: v) {
           s/= w;
           }
           }
           */
        return v;
    }


    template <int N>
        static int get_slash_token(const std::string& str) {
            std::string tok;
            std::stringstream ss(str);
            for(int i = 0; i <= N; ++i) {
                std::getline(ss,tok,'/');
            }
            return std::stod(tok);
        }


    static Tri process_triangle (TokIt begin, const TokIt& end) {
        Tri t;

        std::transform(begin,end,t.begin(),[](const std::string& s) {
                return get_slash_token<0>(s);
                });
        return t;
    }
    static std::tuple<mtao::ColVectors<T,3>,mtao::ColVectors<int,3>> read_obj(const std::string& filename) {
        std::vector<Vec> vecs;
        std::vector<Tri> tris;

        std::ifstream ifs(filename);

        for(std::string line; std::getline(ifs,line);) {
            std::istringstream iss(line);
            std::vector<std::string> tokens;
            std::copy(std::istream_iterator<std::string>(iss)
                    , std::istream_iterator<std::string>()
                    , std::back_insert_iterator<std::vector<std::string> >(tokens));


            if(tokens.empty()) {
                continue;
            } else {
                auto&& front = tokens.front();
                if(front[0] == 'v') {
                    if(front.size() == 1) {//cover for texture coordinates
                        vecs.emplace_back(process_vertex(tokens.begin()+1,tokens.end()));
                    }
                }
                if(front[0] == 'f') {
                    tris.emplace_back(process_triangle(tokens.begin()+1,tokens.end()));
                    if(tokens.size() == 5) {
                        int last = get_slash_token<0>(tokens.back());
                        auto&& p = tris.back();
                        tris.emplace_back(Tri{{p[0],p[2],last}});
                    }
                }
            }
        }

        mtao::ColVectors<T,3> V(3,vecs.size());
        mtao::ColVectors<int,3> F(3,tris.size());

        using Vec3 = Eigen::Matrix<T,3,1>;

        for(int i = 0; i < V.cols(); ++i) {
            V.col(i) = Eigen::Map<Vec3>(&vecs[i].front());
        }

        for(int i = 0; i < F.cols(); ++i) {
            F.col(i) = Eigen::Map<Eigen::Array<int,3,1>>(&tris[i].front()) - 1; //OBJ uses 1 indexing!
        }
        return {V,F};
    }
};

std::tuple<mtao::ColVectors<double,3>,mtao::ColVectors<int,3>> read_objD(const std::string& filename) {
    return MeshReader<double>::read_obj(filename);
}
std::tuple<mtao::ColVectors<float,3>,mtao::ColVectors<int,3>> read_objF(const std::string& filename) {
    return MeshReader<float>::read_obj(filename);
}

}}}
