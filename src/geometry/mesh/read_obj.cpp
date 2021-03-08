#include "mtao/geometry/mesh/read_obj.hpp"
#include "mtao/eigen/stl2eigen.hpp"
#include "mtao/geometry/mesh/earclipping.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <vector>

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
            return get_slash_token<0>(s) - 1;
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
                if (front[0] == ('c' + D) || front[0] == 'l') {
                    if (tokens.size() >= D + 1) {
                        if (tokens.size() == D + 1) {
                            add_valid_simplex(process_simplex<D>(
                              tokens.begin() + 1, tokens.begin() + D + 1));
                        } else {
                            std::vector<int> dat;
                            dat.reserve(tokens.size() - 1);
                            std::transform(tokens.begin() + 1, tokens.end(), std::back_inserter(dat), [&](const std::string &tok) {
                                return get_slash_token<0>(tok) - 1;
                            });

                            if constexpr (D == 2) {
                                for (size_t j = 0; j < dat.size() - 1; ++j) {
                                    add_valid_simplex(Edge{ { dat[j], dat[j + 1] } });
                                }
                            } else if constexpr (D == 3) {// in 3D we also work witih polygons
                                auto V = mtao::eigen::stl2eigen(vecs);
                                auto a = V.col(dat[0]);
                                auto b = V.col(dat[1]);
                                Eigen::Matrix<T, D - 1, D> UV;
                                auto u = UV.row(0) = (b - a).transpose();
                                u.normalize();
                                mtao::Vector<T, 3> N;
                                for (size_t j = 1; j < dat.size(); ++j) {
                                    auto c = V.col(dat[j]);
                                    auto v = UV.row(1) = (c - a).normalized().transpose();
                                    N = u.cross(v);
                                    if (N.norm() > 1e-2) {
                                        break;
                                    }
                                }
                                auto F = mtao::geometry::mesh::earclipping(UV * V, dat);
                                for (int j = 0; j < F.cols(); ++j) {
                                    auto f = F.col(j);
                                    add_valid_simplex(Tri{ { f(0), f(1), f(2) } });
                                }
                            }
                        }
                    }
                }
            }
        }

        return { mtao::eigen::stl2eigen(vecs), mtao::eigen::stl2eigen(tris) };
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
