#pragma once
#include "mtao/geometry/grid/grid.h"
namespace mtao { namespace geometry { namespace grid { namespace stencil {
    template <typename Derived, typename Scalar = Derived::Scalar, int D_ = Derived::D>
        class StencilBase {
            public:
                using derived_type = Derived;
                using value_type = Scalar;
                using D = D_;
                using index_type = mtao::array<int,D>;
                using value_pack = mtao::array<Scalar,D>;
                using GridType = mtao::geometry::grid::Grid<Scalar,D>;

                StencilBase(const GridType& grid): m_grid(grid) {}

                value_pack operator()(IndexType idx) const {
                    value_pack values;
                    auto it = values.begin();
                    mtao::static_loop([&](int dim, const value_type& v) {
                            (*it++) = m_grid(idx);
                            idx[dim]++;
                            (*it++) = m_grid(idx);
                            idx[dim]--;
                            });
                    return evaluate(values);
                }

                value_pack evaluate(const value_pack& values) const {
                    return static_cast<Derived*>(this)->evaluate(values);
                }
            private:
                const mtao::Grid<Scalar,D>& m_grid;

        };
    /*
    template <typename StencilType, typename... Args>
        auto make_stencil(Args&... args) {
        }
        */


}}}}



