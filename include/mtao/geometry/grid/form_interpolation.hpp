#pragma once
#include "mtao/geometry/grid/staggered_grid.hpp"
#include "mtao/geometry/grid/grid_data.hpp"
#include "mtao/geometry/grid/grid_utils.h"
#include "mtao/type_utils.h"


namespace mtao::geometry::grid {
    template <int Dim, typename StaggeredGridType, typename FormStorage>
    class FormInterpolator {
        public:
            using PointType = typename StaggeredGridType::Vec;
            using Scalar = typename StaggeredGridType::Scalar;
            static constexpr int D = StaggeredGridType::D;
            static constexpr int ValSize = StaggeredGridType::template form_grid_size<Dim>();
            static constexpr bool return_scalar() { return ValSize == 1; }
            using VecType = Vector<Scalar,ValSize>;

            //using GridData= GridDataType;

            using RetType = std::conditional_t<return_scalar(), Scalar, VecType>;
            //using FormStorage = std::conditional<return_scalar(), GridType, std::array<GridData,ValSize>>;
            //using GridType = StaggeredGridType::GridType;


            FormInterpolator(const StaggeredGridType& b, const FormStorage& form_data): _base(b), _form_data(form_data) {}

            template <typename Derived>
                RetType operator()(const Eigen::MatrixBase<Derived>& x) const {
                    RetType r;
                    eval(x,r);
                    return r;
                }
            template <typename Derived, typename RetType>
                void eval(const Eigen::MatrixBase<Derived>& x, RetType& r) const {
                    if constexpr(return_scalar()) {
                        r = mtao::lerp(_form_data[0],_base.template grid<Dim,0>().local_coord(x));
                    } else {
                        for(auto&& [i,f,g]: mtao::iterator::enumerate(_form_data,_base.template grids<Dim>())) {
                            r(i) = mtao::lerp(f,g.local_coord(x));
                        }
                    }
                }
        private:
            const StaggeredGridType& _base;
            const FormStorage&_form_data;
    };
    template <int Dim, typename StaggeredGridType, typename GridDataType>
    auto form_interpolator(const StaggeredGridType& s, const GridDataType& g) {
        return FormInterpolator<Dim,StaggeredGridType, GridDataType>(s,g);
    }

}
