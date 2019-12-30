#include <iostream>
#include <catch2/catch.hpp>
#include "mtao/geometry/grid/grid_data.hpp"
#include <mtao/iterator/enumerate.hpp>
#include <utility>
#include <numeric>
#include <set>

template <typename T>
void printGrid2(const T& g) {
    for(int i = 0; i < g.template width<0>(); ++i) {
        for(int j = 0; j < g.template width<1>(); ++j) {
            std::cout << g(i,j) << " ";
        }
        std::cout << std::endl;
    }
}
template <typename T>
void printIndex2(const T& g) {
    for(int i = 0; i < g.template width<0>(); ++i) {
        for(int j = 0; j < g.template width<1>(); ++j) {
            std::cout << g.index(i,j) << " ";
        }
        std::cout << std::endl;
    }
}

auto returnGrid(int i, int j) {
    mtao::geometry::grid::GridData2f a(i,j);
    for(size_t i = 0; i < a.size(); ++i) {
        a.get(i) = i;
    }
    return a;
}
//auto returnVGrid(int i, int j) {
//    mtao::geometry::grid::GridData<Eigen::Vector2f,2> a(i,j);
//    for(int c = 0; c < a.size(); ++c) {
//        a(c) = Eigen::Vector2f(float(c%i),float(j));
//    }
//    return a;
//}
struct A {
    mtao::geometry::grid::GridData2f grid;
};


TEST_CASE("Grid iteration", "[grid]") {
    //mtao::geometry::grid::GridData2f a(std::array<int,2>{{3,5}});
    A a;
    a.grid = returnGrid(3,5);
    //std::cout << std::endl;
    //std::cout << "Retrieval!" << std::endl;

    //for(int j = 0; j < a.shape(1); ++j) {
    //    for(int i = 0; i < a.shape(0); ++i) {
    //        std::cout << a(i,j) << " ";
    //    }
    //}



    //std::cout << std::endl;
    //std::cout << std::endl;
    ////std::cout << std::endl;
    ////std::cout << "Creating b: =================" << std::endl;
    //auto b = mtao::geometry::grid::GridData<Eigen::Vector2f,2>::Constant(Eigen::Vector2f::Ones(),3,5);
    //std::cout << b.rows() << " " << b.cols() << std::endl;
    //std::cout << "b size: " << b.size() << std::endl;
    //printGrid2(b);
    //std::cout << std::endl;
    //auto c = a + b;
    //b = a;


    //printGrid2(b);
    //std::cout << " == " << std::endl;
    //printGrid2(c);
    //printGrid2(a+c);
    //auto d = a;
    //printGrid2(d);

    std::array<int,2> arr;
    arr[0] = 2;
    arr[1] = 3;
    std::set<std::array<int,2>> seen;
    mtao::geometry::grid::utils::multi_loop(arr,[&](auto&& a) {
            seen.insert(a);
            for(auto&& [i,v]: mtao::iterator::enumerate(a)) {
                REQUIRE(v >= 0);
                REQUIRE(v < arr[i]);
            }
            });
    REQUIRE(seen.size() == 2*3);
    seen.clear();
    mtao::geometry::grid::utils::right_multi_loop(arr,[&](auto&& a) {
            seen.insert(a);
            for(auto&& [i,v]: mtao::iterator::enumerate(a)) {
                REQUIRE(v >= 0);
                REQUIRE(v < arr[i]);
            }
            });
    REQUIRE(seen.size() == 2*3);

    a.grid.loop_write_idx([&](auto&& v) -> float {
            return v[0];
            });
    mtao::geometry::grid::utils::multi_loop(arr,[&](auto&& c) {
            REQUIRE(a.grid(c) == c[0]);
            });

    a.grid.loop_write_idx([&](auto&& v) -> float {
            return v[1];
            });
    mtao::geometry::grid::utils::multi_loop(arr,[&](auto&& c) {
            REQUIRE(a.grid(c) == c[1]);
            });


}

TEST_CASE("Grid assignment", "[grid]") {
    auto A = returnGrid(5,5);
    auto B = returnGrid(5,5);
    for(auto&& a: A) {
        a = 1;
    }
    for(auto a: A) {
        REQUIRE(a == 1);
    }
    for(auto&& b: B) {
        b = 2;
    }
    for(auto b: B) {
        REQUIRE(b == 2);
    }
    std::cout << std::endl;
    mtao::geometry::grid::GridData2f C;
    C = (A-B)/3;
    for(auto c: C) {
        REQUIRE(c == Approx(-1/3.));
    }

    C = A - B;
    for(auto c: C) {
        REQUIRE(c == Approx(-1));
    }

}
TEST_CASE("Matlab storage compatibility", "[grid]") {
    mtao::geometry::grid::GridData3i g(10,11,12);
    
    REQUIRE( g.size() == 10*11*12);
    std::iota(g.begin(),g.end(),1);
    /*
    >> x = reshape(1:(10*11*12),[10,11,12]);
    >> [x(1,1,1),x(2,1,1),x(1,2,1),x(1,1,2),x(4,5,6)]

    ans =

         1     2    11   111   594

         */


    REQUIRE(g(0,0,0) == 1 );
    REQUIRE(g(1,0,0) == 2 );
    REQUIRE(g(0,1,0) == 11 );
    REQUIRE(g(0,0,1) == 111 );
    REQUIRE(g(3,4,5) == 594 );

}

