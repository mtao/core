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
        std::array<int,D> r;
        std::iota(r.begin(),r.end(),1);
        std::cout << "====0,0" << std::endl;
        sg.template cell_vertex_looper<0>(0, r, [&](const std::array<int,D>& c) {
                for(auto&& v: c) {
                std::cout << v << ",";
                }
                std::cout << std::endl;
                });
        for(int i = 0; i < D; ++i) {
            std::cout << "====1," << i << std::endl;
            sg.template cell_vertex_looper<1>(i, r, [&](const std::array<int,D>& c) {
                    for(auto&& v: c) {
                    std::cout << v << ",";
                    }
                    std::cout << std::endl;
                    });

            std::cout << " => ";
                    for(auto&& v: sg.edge(i,r)) {
                    std::cout << v << ",";
                    } std::cout << std::endl;
        }
        for(int i = 0; i < (D==2?1:D); ++i) {
            std::cout << "====2," << i << std::endl;
            sg.template cell_vertex_looper<2>(i, r, [&](const std::array<int,D>& c) {
                    for(auto&& v: c) {
                    std::cout << v << ",";
                    }
                    std::cout << std::endl;
                    });
        }
    }

}

int main() {
    test<2>();
    test<3>();
    return 0;
}
