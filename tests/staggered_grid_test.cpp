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
    /*
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
    std::cout << "sizes: " << std::endl;
    std::cout << "Vertex size: " << sg.vertex_size() << std::endl;
    std::cout << "u size: " << sg.u_size() << std::endl;
    std::cout << "v size: " << sg.v_size() << std::endl;
    if constexpr(D == 3) {
    std::cout << "w size: " << sg.w_size() << std::endl;
    }
    std::cout << "edge size: " << sg.edge_size() << std::endl;
    std::cout << "flux_size: " << sg.flux_size() << std::endl;
    }

    */
    {
        mtao::geometry::grid::StaggeredGrid<float,D,true> sg(arr);
        auto ofs = sg.template offsets<1>();
        for(auto&& v: ofs) {
            std::cout << v << "," ;
        }
        std::cout << std::endl;
        auto grids= sg.template grids<1>();
        for(auto&& v: grids) {
            std::cout << v.size()  << "," ;
        }
        std::cout << std::endl;
        auto sizes = std::get<1>(mtao::geometry::grid::staggered_grid::staggered_grid_sizes(sg.shape()));
        for(auto&& v: sizes) {
            std::cout << v << "," ;
        }
        using namespace iterator;

        for(int index = 0; index < sg.template form_size<1>(); ++index) {

            auto [c,ft] = sg.template form_unindex<1>(index);
            std::cout << index <<") " << ft << ": ";


            for(auto&& v: c) {
                std::cout << v << ",";
            }
            std::cout << "=> " << sg.template staggered_index<1>(c,ft);
            std::cout << std::endl;
        }
        std::cout << std::endl;
        auto&& g = sg.template grid<1>(0);
        for(int i = 0; i < g.size(); ++i) {
            std::cout << i << ")";
            auto c = g.unindex(i);
            for(auto&& v: c) {
                std::cout << v << ",";
            }
            std::cout << "=>" << g.index(c) << std::endl;
        }
    }


}

int main() {
    test<2>();
    test<3>();
    return 0;
}
