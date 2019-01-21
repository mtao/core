#include <mtao/geometry/grid/staggered_grid.hpp>
#include <iostream>
#include <numeric>
#include <iterator>
#include <array>

using namespace mtao;

template <int D>
void test() {
    std::cout << "Test" << std::endl;
    std::array<int,D> arr;
    std::iota(arr.begin(),arr.end(),3);
    for(auto&& a: arr) {
        std::cout << a << ",";
    }
    auto printArr = [&](auto&& a) {

        std::copy(a.begin(),a.end(),std::ostream_iterator<int>(std::cout,","));
        std::cout << std::endl;
    };
    auto printG = [&](auto&& gg) 
    {

        auto a = gg.shape();
        auto o = gg.origin();
        auto dx = gg.dx();
        std::cout << "Grid shape: ";
        printArr(a);
        std::cout << o.transpose() << " + " << dx.transpose() << std::endl;
//        std::cout << gg.bbox().min().transpose() << " => " << gg.bbox().max().transpose() << std::endl;
    };
    auto printSG = [&](auto&& sg) {
        std::cout << "BBOX: "<< sg.bbox().min().transpose() << " => " << sg.bbox().max().transpose() << std::endl;
    auto g00 = sg.template grid<0,0>();
    auto g10 = sg.template grid<1,0>();
    auto g11 = sg.template grid<1,1>();
    auto gD0 = sg.template grid<D,0>();
    printG(g00);
    printG(g10);
    printG(g11);
    printG(gD0);
    std::cout << sg.vertices().transpose() << std::endl;
    };
    {
        mtao::geometry::grid::StaggeredGrid<float,D> sg(arr);
        printSG(sg);
    }
    {
        std::cout << "Use vertices shape as shape" << std::endl;
        mtao::geometry::grid::StaggeredGrid<float,D,true> sg(arr);
        printSG(sg);
    }

}

int main() {
    test<2>();
    test<3>();
    return 0;
}
