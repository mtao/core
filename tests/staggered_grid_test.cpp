#include <catch2/catch.hpp>
#include <mtao/geometry/grid/staggered_grid.hpp>
#include <iostream>
#include <numeric>
#include <iterator>
#include <array>
#include <mtao/iterator/enumerate.hpp>
#include <mtao/iterator/reverse.hpp>

using namespace mtao;

template <int D>
void staggered_grid_test() {
    /*
    auto printArr = [&](auto&& a) {

        std::copy(a.begin(),a.end(),std::ostream_iterator<int>(std::cerr,","));
        std::cerr << std::endl;
    };
    auto printG = [&](auto&& gg) 
    {

        auto a = gg.shape();
        auto o = gg.origin();
        auto dx = gg.dx();
        std::cerr << "Grid shape: ";
        printArr(a);
        std::cerr << o.transpose() << " + " << dx.transpose() << std::endl;
//        std::cerr << gg.bbox().min().transpose() << " => " << gg.bbox().max().transpose() << std::endl;
    };
    auto printSG = [&](auto&& sg) {
        std::cerr << "BBOX: "<< sg.bbox().min().transpose() << " => " << sg.bbox().max().transpose() << std::endl;
    auto g00 = sg.template grid<0,0>();
    auto g10 = sg.template grid<1,0>();
    auto g11 = sg.template grid<1,1>();
    auto gD0 = sg.template grid<D,0>();
    printG(g00);
    printG(g10);
    printG(g11);
    printG(gD0);
    std::cerr << sg.vertices().transpose() << std::endl;
    };
    {
        mtao::geometry::grid::StaggeredGrid<float,D> sg(arr);
        printSG(sg);
    }
    {
        std::cerr << "Use vertices shape as shape" << std::endl;
        mtao::geometry::grid::StaggeredGrid<float,D,true> sg(arr);
        printSG(sg);
    std::cerr << "sizes: " << std::endl;
    std::cerr << "Vertex size: " << sg.vertex_size() << std::endl;
    std::cerr << "u size: " << sg.u_size() << std::endl;
    std::cerr << "v size: " << sg.v_size() << std::endl;
    if constexpr(D == 3) {
    std::cerr << "w size: " << sg.w_size() << std::endl;
    }
    std::cerr << "edge size: " << sg.edge_size() << std::endl;
    std::cerr << "flux_size: " << sg.flux_size() << std::endl;
    }

    */
    /*
    {
        auto sizes = std::get<1>(mtao::geometry::grid::staggered_grid::staggered_grid_sizes(sg.cell_shape()));
        for(auto&& v: sizes) {
            std::cerr << v << "," ;
        }
        using namespace iterator;

        for(size_t index = 0; index < sg.template form_size<1>(); ++index) {

            auto [c,ft] = sg.template form_unindex<1>(index);
            std::cerr << index <<") " << ft << ": ";


            for(auto&& v: c) {
                std::cerr << v << ",";
            }
            std::cerr << "=> " << sg.template staggered_index<1>(c,ft);
            std::cerr << std::endl;
        }
        std::cerr << std::endl;
        auto&& g = sg.template grid<1>(0);
        for(size_t i = 0; i < g.size(); ++i) {
            std::cerr << i << ")";
            auto c = g.unindex(i);
            for(auto&& v: c) {
                std::cerr << v << ",";
            }
            std::cerr << "=>" << g.index(c) << std::endl;
        }
    }
    */


}

TEST_CASE("Staggered Grid vertices 2D", "[grid][staggered_grid]") {
    constexpr static int D = 2;
    std::array<int,D> arr;
    std::iota(arr.begin(),arr.end(),3);
    mtao::geometry::grid::StaggeredGrid<float,D> sg(arr);

    auto grids= sg.template grids<1>();
    auto ofs = sg.template offsets<1>();

    REQUIRE(sg.template grids<0>().size() == 1);
    REQUIRE(sg.template grids<1>().size() == 2);
    REQUIRE(sg.template grids<2>().size() == 1);

    assert(sg.origin().isApprox(sg.vertex_grid().origin()));
    auto dx = sg.dx();
    REQUIRE(dx(0) == Approx(1./2));
    REQUIRE(dx(1) == Approx(1./3));

    auto g0 = sg.vertex_grid();
    auto gD = sg.cell_grid();
    for(int j = 0; j < D; ++j) {
        REQUIRE(g0.shape()[j] == arr[j]);
        REQUIRE(g0.origin()(j) == Approx(0));

        REQUIRE(gD.shape()[j] == arr[j]-1);
        REQUIRE(gD.origin()(j) == Approx(.5/(j+2)));
    }

    SECTION("1Form offsets and sizes") {
        for(auto&& [i,v]: mtao::iterator::enumerate(grids)) {
            for(int j = 0; j < D; ++j) {
                if(j == i) {
                    REQUIRE(v.shape()[j] == arr[j]-1);
                    REQUIRE(v.origin()(j) == Approx(.5/(j+2)));
                } else {
                    REQUIRE(v.shape()[j] == arr[j]);
                    REQUIRE(v.origin()(j) == Approx(0));
                }
            }
            REQUIRE(ofs[i+1] - ofs[i] == v.size());
        }

    }
    {
    REQUIRE(sg.form_volumes<0>()[0] == 1);
    auto g = sg.form_volumes<1>();
    REQUIRE(g[0] == dx(0));
    REQUIRE(g[1] == dx(1));
    double pd = dx.prod();
    REQUIRE(sg.form_volumes<2>()[0] == Approx(pd));
    }
    {

    {
        REQUIRE(sg.form_volumes(0)[0] == 1);
        {
            auto g = sg.form_volumes(1);
            REQUIRE(g[0] == dx(0));
            REQUIRE(g[1] == dx(1));
        }
        double pd = dx.prod();
        REQUIRE(sg.form_volumes(2)[0] == Approx(pd));
    }
    }
}
TEST_CASE("Staggered Grid vertices 3D", "[grid][staggered_grid]") {
    constexpr static int D = 3;
    std::array<int,D> arr;
    std::iota(arr.begin(),arr.end(),3);
    mtao::geometry::grid::StaggeredGrid<float,D> sg(arr);
    assert(sg.origin().isApprox(sg.vertex_grid().origin()));
    auto dx = sg.dx();
    REQUIRE(dx(0) == Approx(1./2));
    REQUIRE(dx(1) == Approx(1./3));
    REQUIRE(dx(2) == Approx(1./4));


    REQUIRE(sg.template grids<0>().size() == 1);
    REQUIRE(sg.template grids<1>().size() == 3);
    REQUIRE(sg.template grids<2>().size() == 3);
    REQUIRE(sg.template grids<3>().size() == 1);

    auto g0 = sg.vertex_grid();
    auto gD = sg.cell_grid();
    for(int j = 0; j < D; ++j) {
        REQUIRE(g0.shape()[j] == arr[j]);
        REQUIRE(g0.origin()(j) == Approx(0));

        REQUIRE(gD.shape()[j] == arr[j]-1);
        REQUIRE(gD.origin()(j) == Approx(.5/(j+2)));
    }

    SECTION("1Form offsets and sizes") {
        auto grids= sg.template grids<1>();
        auto ofs = sg.template offsets<1>();
        for(auto&& [i,v]: mtao::iterator::enumerate(grids)) {
            for(int j = 0; j < D; ++j) {
                if(j == i) {
                    REQUIRE(v.shape()[j] == arr[j]-1);
                    REQUIRE(v.origin()(j) == Approx(.5/(j+2)));
                } else {
                    REQUIRE(v.shape()[j] == arr[j]);
                    REQUIRE(v.origin()(j) == Approx(0));
                }
            }
            REQUIRE(ofs[i+1] - ofs[i] == v.size());
        }
    }
    SECTION("2Form offsets and sizes") {
        auto grids= sg.template grids<2>();
        auto ofs = sg.template offsets<2>();
        for(auto&& [i,v]: mtao::iterator::enumerate(grids)) {
            for(int j = 0; j < D; ++j) {
                if(j == i) {
                    REQUIRE(v.shape()[j] == arr[j]);
                    REQUIRE(v.origin()(j) == Approx(0));
                } else {
                    REQUIRE(v.shape()[j] == arr[j]-1);
                    REQUIRE(v.origin()(j) == Approx(.5/(j+2)));
                }
            }
            REQUIRE(ofs[i+1] - ofs[i] == v.size());
        }
    }
    {
        REQUIRE(sg.form_volumes(0)[0] == 1);
        {
            auto g = sg.form_volumes(1);
            REQUIRE(g[0] == dx(0));
            REQUIRE(g[1] == dx(1));
            REQUIRE(g[2] == dx(2));
        }

        {
            auto g = sg.form_volumes(2);
            REQUIRE(g[0] == Approx(dx(1) * dx(2)));
            REQUIRE(g[1] == Approx(dx(0) * dx(2)));
            REQUIRE(g[2] == Approx(dx(0) * dx(1)));
        }
        double pd = dx.prod();

        REQUIRE(sg.form_volumes(3)[0] == Approx(pd));
    }
    {
        REQUIRE(sg.form_volumes<0>()[0] == 1);
        {
            auto g = sg.form_volumes<1>();
            REQUIRE(g[0] == dx(0));
            REQUIRE(g[1] == dx(1));
            REQUIRE(g[2] == dx(2));
        }

        {
            auto g = sg.form_volumes<2>();
            REQUIRE(g[0] == Approx(dx(1) * dx(2)));
            REQUIRE(g[1] == Approx(dx(0) * dx(2)));
            REQUIRE(g[2] == Approx(dx(0) * dx(1)));
        }
        double pd = dx.prod();

        REQUIRE(sg.form_volumes<3>()[0] == Approx(pd));
    }
}
