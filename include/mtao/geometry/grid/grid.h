#pragma once
#include <algorithm>
#include "grid_utils.h"
//#include "grid_storage.h"
#include "indexers/ordered_indexer.hpp"
#include <Eigen/Geometry>


namespace mtao {
    namespace geometry {
        namespace grid {

            //NOTE: The UseVertexGrid flag is mainly for compabitibility with deriving staggered grids from this class
            template <typename T, typename Indexer, bool UseVertexGrid_=true>
                class Grid: public Indexer {
                    public:
                        EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
                        using value_type = T;
                        using Scalar = T;//for eigen compat
                        static constexpr int UseVertexGrid = UseVertexGrid_;
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
                        Grid(const coord_type& a): Grid(a,(1.0 / (CIVecMap(a.data()).template cast<T>().array()-UseVertexGrid)).matrix()) {}
                        Grid() {}
                        static Grid from_bbox(BBox bb, const coord_type& shape) {
                            auto o = bb.min();
                        auto dx = bb.sizes().array() / (CIVecMap(shape.data()).template cast<T>().array()-UseVertexGrid);
                        return Grid(shape,dx.matrix(),o);
                        }
                        Grid(const Grid& other) = default;
                        Grid(Grid&& other) = default;
                        Grid& operator=(const Grid& other) = default;
                        Grid& operator=(Grid&& other) = default;
                        void resize(const coord_type& idx) {
                            Indexer::resize(idx);
                        }

                        BBox bbox() const {
                            if constexpr(UseVertexGrid) {
                                return BBox(origin(), vertex(shapeAsIVec() - IVec::Ones()));
                            } else {
                            return BBox(origin(), vertex(shapeAsIVec()));
                            }
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


                        ColVecs vertices() const {
                            ColVecs V(D,size());
                            utils::multi_loop(shape(),[&](auto&& a) {
                                    V.col(this->index(a)) = vertex(a);
                                    });
                            return V;
                        }

                        auto shapeAsIVec() const {
                            return idx2ivec(shape());
                        }
                        auto&& origin() const { return m_origin; }
                        auto&& dx() const { return m_dx; }

                        template <typename Derived>
                            auto coord(const Eigen::MatrixBase<Derived>& v) const {
                                Vec p = (v - origin()).cwiseQuotient( dx());
                                std::array<int,D> coord;
                                std::array<T,D> quot;
                                IVecMap(coord.data()) = p.template cast<int>();
                                VecMap(quot.data()) = p - IVecMap(coord.data()).template cast<T>();
                                return std::make_tuple(coord,quot);
                            }


                    private:
                        Vec m_origin = Vec::Zero();
                        Vec m_dx = Vec::Ones();


                };


            template <typename T, int D, bool UseVertexGrid=true>
                using GridD = Grid<T,indexing::OrderedIndexer<D>,UseVertexGrid>;
            using Grid2f = GridD<float,2>;
            using Grid3f = GridD<float,3>;
            using Grid2i = GridD<int,2>;
            using Grid3i = GridD<int,3>;
            using Grid2d = GridD<double,2>;
            using Grid3d = GridD<double,3>;
        }

    }
}

