#pragma once
#include <mtao/types.hpp>
#include <numeric>
#include <optional>
namespace mtao::geometry::mesh::triangle {
#ifdef SINGLE
using REAL = float;
#else /* not SINGLE */
using REAL = double;
#endif /* not SINGLE */

struct Mesh {
    Mesh() = default;
    Mesh(Mesh &&) = default;
    Mesh(const Mesh &) = default;
    Mesh(const mtao::ColVectors<REAL, 2> V,
         const mtao::ColVectors<int, 2> E = mtao::ColVectors<int, 2>(),
         const mtao::ColVectors<int, 3> F = mtao::ColVectors<int, 3>());
    Mesh(const std::tuple<mtao::ColVectors<REAL, 2>, mtao::ColVectors<int, 2>> &VE);
    Mesh &operator=(const Mesh &other) = default;
    Mesh &operator=(Mesh &&other) = default;
    void fill_attributes();
    mtao::ColVectors<REAL, 2> V;//vertices
    mtao::ColVectors<int, 2> E;//edges
    mtao::VectorX<int> VA;//vertex markers
    mtao::VectorX<int> EA;//edge markers
    mtao::ColVectors<int, 3> F;//faces
    mtao::ColVectors<REAL, 2> H;//Point in a hole
    std::optional<mtao::ColVectors<REAL, 2>> C;//circumcenters

    mtao::VectorX<bool> verify_delauney() const;

    void translate(const mtao::Vector<REAL, 2> &);
    void scale(const mtao::Vector<REAL, 2> &);
    void make_circumcenters();
};

Mesh combine(const std::vector<Mesh> &components);

template<typename T, typename FuncType>
std::tuple<T, std::vector<int>> vector_hstack(const std::vector<Mesh> &data, const FuncType &accessor) {

    std::vector<int> sizes;
    std::transform(data.begin(), data.end(), std::back_inserter(sizes), [&](const Mesh &m) -> int {
        return accessor(m).cols();
    });
    std::vector<int> offsets(1, 0);
    std::partial_sum(sizes.begin(), sizes.end(), std::back_inserter(offsets));
    T H(accessor(data[0]).rows(), offsets.back());
    for (size_t i = 0; i < data.size(); ++i) {
        auto &&h = accessor(data[i]);
        H.block(0, offsets[i], h.rows(), h.cols()) = h;
    }
    return { H, offsets };
}

template<typename T, typename FuncType>
std::tuple<T, std::vector<int>> vector_vstack(const std::vector<Mesh> &data, const FuncType &accessor) {

    std::vector<int> sizes;
    std::transform(data.begin(), data.end(), std::back_inserter(sizes), [&](const Mesh &m) -> int {
        return accessor(m).rows();
    });
    std::vector<int> offsets(1, 0);
    std::partial_sum(sizes.begin(), sizes.end(), std::back_inserter(offsets));
    T H(offsets.back(), accessor(data[0]).cols());
    for (size_t i = 0; i < data.size(); ++i) {
        auto &&h = accessor(data[i]);
        H.block(offsets[i], 0, h.rows(), h.cols()) = h;
    }
    return { H, offsets };
}
}// namespace mtao::geometry::mesh::triangle
