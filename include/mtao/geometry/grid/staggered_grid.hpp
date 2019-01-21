#pragma once
#include "mtao/algebra/combinatorial.hpp"
#include "staggered_grid_utils.hpp"
#include "grid.h"
#include <tuple>

namespace mtao {
    namespace geometry {
        namespace grid {

            template <typename T, int Dim>
                struct StaggeredGrid: public GridD<T,Dim> {
                   
                    public:
                        using GridType = GridD<T,Dim>;
                        using Base = GridType;
                        using index_type = typename Base::index_type;
                        using StaggeredGrids = decltype(staggered_grid::make_grids(std::declval<Base>()));
                        template <typename... Args> 
                            StaggeredGrid(const index_type& shape, Args&&... args): Base(shape, std::forward<Args>(args)...) {
                                resize_grids();
                            }
                        StaggeredGrid() {}
                        StaggeredGrid(const StaggeredGrid& other) = default;
                        StaggeredGrid(StaggeredGrid&& other) = default;
                        StaggeredGrid& operator=(const StaggeredGrid& other) = default;
                        StaggeredGrid& operator=(StaggeredGrid&& other) = default;
                        template <int N, int K>
                            const GridType& grid() {
                            return std::get<K>(std::get<N>(m_grids)); 
                            }

                    private:
                        void resize_grids() {
                            m_grids = staggered_grid::make_grids(*static_cast<Base*>(this));
                        }
                        StaggeredGrids m_grids;
                        

                };

        }
    }
}
