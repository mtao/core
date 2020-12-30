#include "mtao/geometry/mesh/read_obj.hpp"

#include <array>
#include <fstream>
#include <iterator>

#include "mtao/logging/logger.hpp"

using namespace mtao::logging;

namespace {
template<typename T>
struct MeshReader {
    using Vec = std::array<T, 3>;
    using Tri = std::array<int, 3>;
    using Edge = std::array<int, 2>;
    using TokIt = std::vector<std::string>::const_iterator;

    template<int D>

    static std::array<T, D> process_vertex(TokIt begin, const TokIt &end) {
        std::array<T, D> v;
        std::transform(begin, end, v.begin(), [](const std::string &s) { return std::stod(s); });

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

    template<int N>
    static int get_slash_token(const std::string &str) {
        std::string tok;
        std::stringstream ss(str);
        for (int i = 0; i <= N; ++i) {
            std::getline(ss, tok, '/');
        }
        return std::stod(tok);
    }
    template<int D>
    static std::array<int, D> process_simplex(TokIt begin, const TokIt &end) {
        std::array<int, D> t;
        std::transform(begin, end, t.begin(), [](const std::string &s) {
            return get_slash_token<0>(s);
        });
        return t;
    }

    static Edge process_edge(TokIt begin, const TokIt &end) {
        return process_simplex<2>(begin, end);
    }

    static Tri process_triangle(TokIt begin, const TokIt &end) {
        return process_simplex<3>(begin, end);
    }

    template<int D>
    static std::tuple<mtao::ColVectors<T, D>, mtao::ColVectors<int, D>>
      read_objD(std::istream &ifs) {
        using Simp = std::array<int, D>;
        using Vec = std::array<T, D>;
        std::vector<Vec> vecs;
        std::vector<Simp> tris;

        auto add_valid_simplex = [&](const Simp &t) {
            auto check = [](const std::array<int, D> &s) -> bool {
                for (int j = 0; j < D - 1; ++j) {
                    for (int k = j + 1; k < D; ++k) {
                        if (s[j] == s[k]) {
                            return false;
                        }
                    }
                }
                return true;
            };
            if (check(t)) {
                tris.emplace_back(t);
            }
        };

        for (std::string line; std::getline(ifs, line);) {
            std::istringstream iss(line);
            std::vector<std::string> tokens;
            std::copy(
              std::istream_iterator<std::string>(iss),
              std::istream_iterator<std::string>(),
              std::back_insert_iterator<std::vector<std::string>>(tokens));

            if (tokens.empty()) {
                continue;
            } else {
                auto &&front = tokens.front();
                if (front[0] == 'v') {
                    if (front.size() == 1) {// cover for texture coordinates
                        vecs.emplace_back(process_vertex<D>(tokens.begin() + 1,
                                                            tokens.end()));
                    }
                }
                // 'c' + 2 == 'e'; 'c' + 3 == f, so this works!
                if (front[0] == ('c' + D)) {
                    if (tokens.size() >= D + 1) {
                        add_valid_simplex(process_simplex<D>(
                          tokens.begin() + 1, tokens.begin() + D + 1));
                        if constexpr (D == 3) {// in 3D we also work witih quads
                            if (tokens.size() == D + 2) {
                                int last = get_slash_token<0>(tokens.back());
                                auto &&p = tris.back();
                                add_valid_simplex(Tri{ { p[0], p[2], last } });
                            }
                        }
                    }
                }
            }
        }

        mtao::ColVectors<T, D> V(D, vecs.size());
        mtao::ColVectors<int, D> F(D, tris.size());

        using VecD = Eigen::Matrix<T, D, 1>;

        for (int i = 0; i < V.cols(); ++i) {
            V.col(i) = Eigen::Map<VecD>(&vecs[i].front());
        }

        for (int i = 0; i < F.cols(); ++i) {
            F.col(i) = Eigen::Map<Eigen::Array<int, D, 1>>(&tris[i].front()) - 1;// OBJ uses 1 indexing!
        }
        return { V, F };
    }

    template<int D>
    static std::tuple<mtao::ColVectors<T, D>, mtao::ColVectors<int, D>>
      read_objD(const std::string &filename) {
        std::ifstream ifs(filename);
        return read_objD<D>(ifs);
    }
};
}// namespace

namespace mtao::geometry::mesh {
std::tuple<mtao::ColVectors<double, 3>, mtao::ColVectors<int, 3>> read_objD(
  const std::string &filename) {
    return MeshReader<double>::read_objD<3>(filename);
}
std::tuple<mtao::ColVectors<float, 3>, mtao::ColVectors<int, 3>> read_objF(
  const std::string &filename) {
    return MeshReader<float>::read_objD<3>(filename);
}

std::tuple<mtao::ColVectors<double, 2>, mtao::ColVectors<int, 2>> read_obj2D(
  const std::string &filename) {
    return MeshReader<double>::read_objD<2>(filename);
}
std::tuple<mtao::ColVectors<float, 2>, mtao::ColVectors<int, 2>> read_obj2F(
  const std::string &filename) {
    return MeshReader<float>::read_objD<2>(filename);
}

std::tuple<mtao::ColVectors<double, 3>, mtao::ColVectors<int, 3>> read_objD(
  std::istream &is) {
    return MeshReader<double>::read_objD<3>(is);
}
std::tuple<mtao::ColVectors<float, 3>, mtao::ColVectors<int, 3>> read_objF(
  std::istream &is) {
    return MeshReader<float>::read_objD<3>(is);
}

std::tuple<mtao::ColVectors<double, 2>, mtao::ColVectors<int, 2>> read_obj2D(
  std::istream &is) {
    return MeshReader<double>::read_objD<2>(is);
}
std::tuple<mtao::ColVectors<float, 2>, mtao::ColVectors<int, 2>> read_obj2F(
  std::istream &is) {
    return MeshReader<float>::read_objD<2>(is);
}
}// namespace mtao::geometry::mesh
