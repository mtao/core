#pragma once
#include <algorithm>
#include "grid_utils.h"
//#include "grid_storage.h"
#include "indexers/ordered_indexer.hpp"
#include <Eigen/Geometry>
#include "mtao/eigen/stl2eigen.hpp"


namespace mtao {
    namespace geometry {
        namespace grid {

            template <typename T, typename Indexer_>
                class Grid: public Indexer_ {
                    public:
                        using Indexer = Indexer_;
                        EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
                        using value_type = T;
                        using Scalar = T;//for eigen compat
                        static constexpr int D = Indexer::D;
                        using coord_type = typename Indexer::coord_type;
                        using Indexer::shape;
                        using Indexer::size;
                        using Indexer::index;
                        using Indexer::unindex;
                        using Indexer::valid_index;
                        using Indexer::width;
                        using Vec = Vector<T,D>;
                        using ColVecs = ColVectors<T,D>;
                        using VecMap = Eigen::Map<Vec>;
                        using CVecMap = Eigen::Map<const Vec>;

                        using index_scalar_type = typename coord_type::value_type;
                        using IVec = Vector<index_scalar_type,D>;
                        using IVecMap = Eigen::Map<IVec>;
                        using CIVecMap = Eigen::Map<const IVec>;

                        static auto idx2ivec(const coord_type& idx) {
                            return CIVecMap(idx.data());
                        }


                        using BBox = Eigen::AlignedBox<T,D>;
                        

                        template <typename Derived=Vec, typename Derived2>
                        Grid(const coord_type& a, const Eigen::MatrixBase<Derived>& dx, const Eigen::MatrixBase<Derived2>& origin = Vec::Zero()): Indexer(a), m_origin(origin), m_dx(dx) {}
                        template <typename Derived=Vec>
                        Grid(const coord_type& a, const Eigen::MatrixBase<Derived>& dx, const Vec& origin = Vec::Zero()): Indexer(a), m_origin(origin), m_dx(dx) {}
                        Grid(const coord_type& a): Grid(a,(1.0 / (CIVecMap(a.data()).template cast<T>().array()-1)).matrix()) {}
                        Grid() {}
                        static Grid old_bad_from_bbox(BBox bb, const coord_type& shape, bool cubes = false) {
                            auto o = bb.min();
                            auto dx = (bb.sizes().array() / (CIVecMap(shape.data()).template cast<T>().array()-1)).eval();
                            if(cubes) {
                                dx.setConstant(dx.maxCoeff());
                            }
                            return Grid(shape,dx.matrix(),o);
                        }
                        static Grid from_bbox(BBox bb, const coord_type& shape, bool cubes = false) {
                            Vec mid =(bb.min() + bb.max()) / 2;
                            auto dx = (bb.sizes().array() / (CIVecMap(shape.data()).template cast<T>().array()-1)).eval();
                            if(cubes) {
                                dx.setConstant(dx.maxCoeff());
                                bb.max() = (dx.array() * (CIVecMap(shape.data()).template cast<T>().array()-1)).eval() / 2;
                                bb.min() = -bb.max(); 
                                bb.min() += mid;
                                bb.max() += mid;
                            }
                            auto o = bb.min();
                            return Grid(shape,dx.matrix(),o);
                        }
                        Grid(const Grid& other) = default;
                        Grid(Grid&& other) = default;
                        Grid& operator=(const Grid& other) = default;
                        Grid& operator=(Grid&& other) = default;

                    public:
                        template <typename U, typename Idxr>
                        Grid(const Grid<U,Idxr> o): Grid(o.shape(), o.dx().template cast<T>(), o.origin().template cast<T>()) {}

                        template <typename U>
                            Grid<U,Indexer> cast() const {
                                return Grid<U,Indexer>(shape(), dx().template cast<U>(), origin().template cast<U>()); 
                            }


                        void resize(const coord_type& idx) {
                            Indexer::resize(idx);
                        }

                        BBox bbox() const {
                            return BBox(origin(), vertex(shapeAsIVec() - IVec::Ones()));
                        }

                        template <typename Derived>
                        Vec vertex(const Eigen::MatrixBase<Derived>& idx) const {
                            static_assert(Derived::RowsAtCompileTime == D);
                            static_assert(std::is_same_v<typename Derived::Scalar,index_scalar_type>);
                            return origin() + dx().asDiagonal() * (idx.template cast<T>());
                        }
                        Vec vertex(const coord_type& idx) const {
                            return vertex(idx2ivec(idx));
                        }
                        Vec vertex(int idx) const {
                            return vertex(unindex(idx));
                        }

                        template <typename Func>
                            void loop(Func&& f) const {
                                utils::multi_loop(shape(),f);
                            }

                        template <typename Func>
                            void loop_parallel(Func&& f) const {
                                utils::multi_loop_parallel(shape(),f);
                            }
                        ColVecs vertices() const {
                            ColVecs V(D,size());
                            loop_parallel([&](auto&& a) {
                                    V.col(this->index(a)) = vertex(a);
                                    });
                            return V;
                        }

                        ColVecs local_vertices() const {
                            ColVecs V(D,size());
                            loop_parallel([&](auto&& a) {
                                    V.col(this->index(a)) = eigen::stl2eigen(a).template cast<T>();
                                    });
                            return V;
                        }


                        auto shapeAsIVec() const {
                            return idx2ivec(shape());
                        }
                        const Vec& origin() const { return m_origin; }
                        const Vec& dx() const { return m_dx; }

                        //maps world space to local space
                        template <typename Derived>
                            auto local_coord(const Eigen::MatrixBase<Derived>& v) const -> auto {

                                if constexpr(Derived::ColsAtCompileTime == 1) {
                                    return Vec((v - origin()).cwiseQuotient( dx()));
                                } else {
                                        ColVecs R = (v.colwise() - origin()).array().colwise() / dx().array();
                                        return R;
                                }
                            }
                        //maps world space to local space
                        template <typename Derived>
                            auto world_coord(const Eigen::MatrixBase<Derived>& v) const -> auto {

                                if constexpr(Derived::ColsAtCompileTime == 1) {
                                    return Vec(dx().asDiagonal() * v + origin());
                                } else {
                                    ColVecs R = (dx().asDiagonal() * v).colwise() + origin();
                                    return R;
                                }
                            }
                        T local_coord(T v, int axis) const {
                            return (v -  origin()(axis)) / dx()(axis);
                        }
                        T world_coord(T v, int axis) const {
                            return dx()(axis) * v + origin()(axis);
                        }

                        template <typename Derived>
                            auto coord_from_local(const Eigen::MatrixBase<Derived>& p) const {
                                std::array<int,D> coord;
                                std::array<T,D> quot;
                                IVecMap(coord.data()) = p.template cast<int>();
                                VecMap(quot.data()) = p - IVecMap(coord.data()).template cast<T>();
                                return std::make_tuple(coord,quot);
                            }
                        template <typename Derived>
                            auto coord(const Eigen::MatrixBase<Derived>& v) const {
                                auto p = local_coord(v);
                                return coord_from_local(p);
                            }


                    private:
                        Vec m_origin = Vec::Zero();
                        Vec m_dx = Vec::Ones();


                };


            template <typename T, int D>
                using GridD = Grid<T,indexing::OrderedIndexer<D>>;
            using Grid2f = GridD<float,2>;
            using Grid3f = GridD<float,3>;
            using Grid2i = GridD<int,2>;
            using Grid3i = GridD<int,3>;
            using Grid2d = GridD<double,2>;
            using Grid3d = GridD<double,3>;
        }

    }
}

