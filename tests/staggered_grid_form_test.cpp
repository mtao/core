#include <mtao/geometry/grid/staggered_grid.hpp>
#include <mtao/geometry/grid/grid_data.hpp>
#include <iostream>
#include <numeric>
#include <iterator>
#include <array>
#include <mtao/iterator/enumerate.hpp>
#include <mtao/iterator/reverse.hpp>
#include <mtao/geometry/grid/form_interpolation.hpp>

using namespace mtao;


template <typename T>
void printGrid2(const T& g) {
    for(int i = 0; i < g.template width<0>(); ++i) {
        for(int j = 0; j < g.template width<1>(); ++j) {
            std::cout << g(i,j) << " ";
        }
        std::cout << std::endl;
    }
}

template <int Dim, typename StaggeredGridType>
void run_test(const StaggeredGridType& g) {
    auto grids = g.template grids<Dim>();
    constexpr static int D = std::tuple_size<std::decay_t<decltype(grids)>>();
    std::cout << Dim << ": " << D << std::endl;
    using GDType = mtao::geometry::grid::GridDataD<typename StaggeredGridType::Scalar,StaggeredGridType::D>;
    std::array<GDType,D> data;
    std::copy(grids.begin(),grids.end(),data.begin());
    for(auto&& [i,f,g]: mtao::iterator::enumerate(data,grids)) {
        f.as_eigen_vector() = g.vertices().row(i).transpose();
        std::cout << mtao::eigen::stl2eigen(f.shape()).transpose() << std::endl;
    }
    //mtao::geometry::grid::form_interpolator<Dim>(g,data)(mtao::Vec2f(.5,.5));
    std::cout << "Value: " << std::endl;
    std::cout << mtao::geometry::grid::form_interpolator<Dim>(g,data)(StaggeredGridType::Vec::Constant(.5)) << std::endl;
    std::cout << "===" << std::endl;

    
}

template <int D>
void test() {
    std::array<int,D> arr;
    std::iota(arr.begin(),arr.end(),3);
    mtao::geometry::grid::StaggeredGrid<float,D,true> sg(arr);
    run_test<0>(sg);
    run_test<1>(sg);
    run_test<2>(sg);
    if constexpr(D == 3) {
        run_test<3>(sg);
    }

}

int main() {
    test<2>();
    test<3>();
    return 0;
}
