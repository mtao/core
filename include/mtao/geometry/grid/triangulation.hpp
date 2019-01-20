#pragma once
#include "mtao/types.hpp"
#include "mtao/geometry/grid/indexers/ordered_indexer.hpp"

//Util for visualizing grids as triangle meshes


namespace mtao { namespace geometry { namespace grid {
    template <typename Indexer=indexing::OrderedIndexer<3>>
    class GridTopologicalTriangulation: public Indexer {
        public:
            using Indexer::Indexer;
            using Indexer::index;
            using Indexer::shape;
            constexpr static int D = Indexer::D;
            using index_type = typename Indexer::index_type;
            ColVectors<unsigned int, 3> faces() const;
            ColVectors<unsigned int, 2> edges() const;

        private:

    };
    template <typename T, typename Indexer=indexing::OrderedIndexer<3>>
    class GridTriangulation: public GridTopologicalTriangulation<Indexer>  {
        public:
        using Base = GridTopologicalTriangulation<Indexer>;
        constexpr static int D = Base::D;
        using Base::Base;
        using Base::index;
        using Base::shape;
        using Base::size;
        public:
            ColVectors<T,D> vertices() const;
    };
}}}

#include "triangulation_impl.hpp"
