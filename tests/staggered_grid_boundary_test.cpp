#include <mtao/geometry/grid/staggered_grid.hpp>
#include <iostream>
#include <numeric>
#include <iterator>
#include <array>
#include <mtao/iterator/enumerate.hpp>
#include <mtao/iterator/reverse.hpp>

using namespace mtao;

template <int D>
void test() {
    std::cout << "Test" << std::endl;
    std::array<int,D> arr;
    std::iota(arr.begin(),arr.end(),3);
    for(auto&& a: arr) {
        std::cout << a << ",";
    }
    std::cout << std::endl;
    {
        mtao::geometry::grid::StaggeredGrid<float,D,true> sg(arr);
        std::cout << sg.template form_size<D>() << "=>";
        std::cout << sg.template form_size<D-1>() << std::endl;
        std::cout << std::endl;
        std::cout << sg.template boundary<D>() << std::endl;
        std::cout << sg.template form_size<1>() << "=>";
        std::cout << sg.template form_size<0>() << std::endl;
        std::cout << sg.template boundary<1>().transpose() << std::endl;
    }

}

int main() {
    test<2>();
    test<3>();
    return 0;
}
