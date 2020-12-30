#ifndef MESH_CONSTRUCTOR_H
#define MESH_CONSTRUCTOR_H
#include <vector>
#include "types.h"

namespace mtao {

template<typename DerivedType_>
class TriangleMeshBase {
  public:
    using DerivedType = DerivedType_;
    using Dim = DerivedType::Dim;
    using Scalar = typename DerivedType::Scalar;
    using type_selection = mtao::numerical_types<T, Dim>;
    using Vec = typename type_selection::Vec;
    void add_triangle(size_t a, size_t b, size_t c) {
        return static_cast<DerivedType *>(this)->add_triangle(a, b, c);
    }
    void add_vertex(const Vec &v) {
        return static_cast<DerivedType *>(this)->add_vertex(v);
    }

    void add_vertex(std::initalizer_list<Scalar> args) {
        return static_cast<DerivedType *>(this)->add_vertex(Vec(args...));
    }
    void add_vertex(Scalar a, Scalar b) {
        static_assert(Dim == 2, "Dim must match!");
        return static_cast<DerivedType *>(this)->add_vertex(Vec(a, b));
    }
    void add_vertex(Scalar a, Scalar b, Scalar c) {
        static_assert(Dim == 3, "Dim must match!");
        return static_cast<DerivedType *>(this)->add_vertex(Vec(a, b, c));
    }
};
template<typename DerivedType_>
class HeterogeneousMeshBase {
  public:
    using DerivedType = DerivedType_;
    using Scalar = typename DerivedType::Scalar;
    template<size_t...>
    void add_polygon(size_t a, size_t b, size_t c) {
        return static_cast<DerivedType *>(this)->add_triangle(a, b, c);
    }
    void add_vertex(Scalar a, Scalar b) {
        return static_cast<DerivedType *>(this)->add_vertex(a, b, c);
    }
    void add_vertex(Scalar a, Scalar b, Scalar c) {
        return static_cast<DerivedType *>(this)->add_vertex(a, b, c);
    }
};
template<typename MeshType_>
class MeshConstructor {
  public:
    using MeshType = MeshType_;
    using BaseType = MeshBase<MeshType>;
    using Scalar = typename MeshType::Scalar;
    void loadOBJ(const std::string &filename);

  private:
    MeshType m_mesh;

    void parseLine(const std::vector<std::string> &line);
    void parseVertex(const std::vector<std::string> &line);
    void parseFace(const std::vector<std::string> &line);
};
}// namespace mtao

#endif//MESH_CONSTRUCTOR_H
