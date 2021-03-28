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
    using Seg = std::array<int, 2>;
    using TokIt = std::vector<std::string>::const_iterator;

    template<int D>

    static std::array<T, D> process_vertex(TokIt begin, const TokIt &end) {
        std::array<T, D> v;
        auto dist = std::distance(begin, end);
        if (dist > D) {
            std::transform(begin, begin + D, v.begin(), [](const std::string &s) { return std::stod(s); });
        } else {
            std::transform(begin, end, v.begin(), [](const std::string &s) { return std::stod(s); });
            if (dist < D) {
                std::fill(v.begin() + dist, v.end(), 0);
            }
        }

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

    static Seg process_edge(TokIt begin, const TokIt &end) {
        return process_simplex<2>(begin, end);
    }

    static Tri process_triangle(TokIt begin, const TokIt &end) {
        return process_simplex<3>(begin, end);
    }

    template<int D>
    static std::tuple<mtao::ColVectors<T, D>, mtao::ColVectors<int, 3>, mtao::ColVectors<int, 2>>
      read_objD(std::istream &ifs) {
        using Vec = std::array<T, D>;
        std::vector<Vec> vecs;
        std::vector<Tri> tris;
        std::vector<Seg> segs;

        auto add_valid_simplex = [&](const auto &t) {
            using AT = std::decay_t<decltype(t)>;
            auto check = [](const auto &s) -> bool {
                constexpr static int size = std::tuple_size_v<AT>;
                for (int j = 0; j < size - 1; ++j) {
                    for (int k = j + 1; k < size; ++k) {
                        if (s[j] == s[k]) {
                            return false;
                        }
                    }
                }
                return true;
            };
            if (check(t)) {
                if constexpr (std::is_same_v<AT, Tri>) {
                    tris.emplace_back(t);
                } else if constexpr (std::is_same_v<AT, Seg>) {
                    segs.emplace_back(t);
                }
            }
        };

        auto process_a_simplex = [&](const std::vector<std::string> &tokens, auto &&dim) {
            constexpr static int Dim = std::decay_t<decltype(dim)>::value;
            if (tokens.size() >= Dim + 1) {
                if (tokens.size() == Dim + 1) {
                    add_valid_simplex(process_simplex<Dim>(
                      tokens.begin() + 1, tokens.begin() + Dim + 1));
                } else {
                    std::vector<int> dat;
                    dat.reserve(tokens.size() - 1);
                    std::transform(tokens.begin() + 1, tokens.end(), std::back_inserter(dat), [&](const std::string &tok) {
                        return get_slash_token<0>(tok) - 1;
                    });
                    if (int index = *std::max_element(dat.begin(), dat.end()); index >= int(vecs.size())) {
                        // output the original 1-indexed value
                        for (auto &&v : dat) {
                            v++;
                        }
                        throw std::out_of_range(fmt::format("read_obj recieved a request to access for index {} in face with indices {} but has only seen {} vertices thusfar (line was [{}])", index, fmt::join(dat, ","), vecs.size(), fmt::join(tokens, " ")));
                    }

                    if constexpr (Dim == 2) {
                        for (size_t j = 0; j < dat.size() - 1; ++j) {
                            add_valid_simplex(Seg{ { dat[j], dat[j + 1] } });
                        }
                    } else if constexpr (Dim == 3) {// in 3D we also work witih polygons
                        auto V = mtao::eigen::stl2eigen(vecs);

                        mtao::ColVectors<int, 3> F;
                        if constexpr (D == 3) {
                            auto a = V.col(dat[0]);
                            auto b = V.col(dat[1]);
                            Eigen::Matrix<T, 2, Dim> UV;
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
                            F = mtao::geometry::mesh::earclipping(UV * V, dat);
                        } else if constexpr (D == 2) {
                            F = mtao::geometry::mesh::earclipping(V, dat);
                        }
                        for (int j = 0; j < F.cols(); ++j) {
                            auto f = F.col(j);
                            add_valid_simplex(Tri{ { f(0), f(1), f(2) } });
                        }
                    }
                }
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
                if (front[0] == 'e' || front[0] == 'l') {

                    process_a_simplex(tokens, std::integral_constant<int, 2>{});
                } else if (front[0] == 'f') {
                    process_a_simplex(tokens, std::integral_constant<int, 3>{});
                }
            }
        }

        return { mtao::eigen::stl2eigen(vecs), mtao::eigen::stl2eigen(tris), mtao::eigen::stl2eigen(segs) };
    }

    template<int D>
    static std::tuple<mtao::ColVectors<T, D>, mtao::ColVectors<int, 3>, mtao::ColVectors<int, 2>>
      read_objD(const std::string &filename) {
        std::ifstream ifs(filename);
        return read_objD<D>(ifs);
    }
};
}// namespace

namespace mtao::geometry::mesh {
std::tuple<mtao::ColVectors<double, 3>, mtao::ColVectors<int, 3>> read_objD(
  const std::string &filename) {
    auto [V, F, E] = read_objD_with_edges(filename);
    return { V, F };
}
std::tuple<mtao::ColVectors<float, 3>, mtao::ColVectors<int, 3>> read_objF(
  const std::string &filename) {
    auto [V, F, E] = read_objF_with_edges(filename);
    return { V, F };
}

std::tuple<mtao::ColVectors<double, 3>, mtao::ColVectors<int, 3>, mtao::ColVectors<int, 2>> read_objD_with_edges(
  const std::string &filename) {
    return MeshReader<double>::read_objD<3>(filename);
}
std::tuple<mtao::ColVectors<float, 3>, mtao::ColVectors<int, 3>, mtao::ColVectors<int, 2>> read_objF_with_edges(
  const std::string &filename) {
    return MeshReader<float>::read_objD<3>(filename);
}

std::tuple<mtao::ColVectors<double, 2>, mtao::ColVectors<int, 2>> read_obj2D(
  const std::string &filename) {
    auto [V, F, E] = MeshReader<double>::read_objD<2>(filename);
    return { V, E };
}
std::tuple<mtao::ColVectors<float, 2>, mtao::ColVectors<int, 2>> read_obj2F(
  const std::string &filename) {
    auto [V, F, E] = MeshReader<float>::read_objD<2>(filename);
    return { V, E };
}

std::tuple<mtao::ColVectors<double, 3>, mtao::ColVectors<int, 3>, mtao::ColVectors<int, 2>> read_objD(
  std::istream &is) {
    return MeshReader<double>::read_objD<3>(is);
}
std::tuple<mtao::ColVectors<float, 3>, mtao::ColVectors<int, 3>, mtao::ColVectors<int, 2>> read_objF(
  std::istream &is) {
    return MeshReader<float>::read_objD<3>(is);
}

std::tuple<mtao::ColVectors<double, 2>, mtao::ColVectors<int, 2>> read_obj2D(
  std::istream &is) {
    auto [V, F, E] = MeshReader<double>::read_objD<2>(is);
    return { V, E };
}
std::tuple<mtao::ColVectors<float, 2>, mtao::ColVectors<int, 2>> read_obj2F(
  std::istream &is) {
    auto [V, F, E] = MeshReader<float>::read_objD<2>(is);
    return { V, E };
}
}// namespace mtao::geometry::mesh
