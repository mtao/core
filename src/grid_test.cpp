#include <iostream>
#include "grid.h"


template <typename T>
void printGrid2(const T& g) {
    for(int i = 0; i < g.template width<0>(); ++i) {
        for(int j = 0; j < g.template width<1>(); ++j) {
            std::cout << g(i,j) << " ";
        }
        std::cout << std::endl;
    }
}

auto returnGrid(int i, int j) {
    mtao::Grid<float,2> a(i,j);
    for(int i = 0; i < a.size(); ++i) {
        a(i) = i;
    }
    return a;
}
//auto returnVGrid(int i, int j) {
//    mtao::Grid<Eigen::Vector2f,2> a(i,j);
//    for(int c = 0; c < a.size(); ++c) {
//        a(c) = Eigen::Vector2f(float(c%i),float(j));
//    }
//    return a;
//}
struct A {
    mtao::Grid<float,2> grid;
};


void testA() {
    //mtao::Grid<float,2> a(mtao::compat::array<int,2>{{3,5}});
    A a;
    a.grid = returnGrid(3,5);
    printGrid2(a.grid);
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
    //auto b = mtao::Grid<Eigen::Vector2f,2>::Constant(Eigen::Vector2f::Ones(),3,5);
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

    mtao::compat::array<int,3> arr;
    arr[0] = 2;
    arr[1] = 3;
    arr[2] = 4;
    mtao::multi_loop(arr,[](auto&& a) {
            for(auto&& v: a) {
            std::cout << v << " ";
            }
            std::cout << std::endl;
            });
    mtao::right_multi_loop(arr,[](auto&& a) {
            for(auto&& v: a) {
            std::cout << v << " ";
            }
            std::cout << std::endl;
            });

    a.grid.loop_write_idx([&](auto&& v) -> float {
            return v[0];
            });
    printGrid2(a.grid);
    a.grid.loop_write_idx([&](auto&& v) -> float {
            return v[1];
            });
    printGrid2(a.grid);


}

void testB() {
    auto A = returnGrid(5,5);
    auto B = returnGrid(5,5);
    for(auto&& a: A) {
        a = 1;
    }
    for(auto&& b: B) {
        b = 2;
    }
    printGrid2(A);
    std::cout << std::endl;
    printGrid2(B);
    std::cout << std::endl;
    mtao::Grid<float,2> C;
    C = (A-B)/3;
    printGrid2(A-B);
    printGrid2(C);

}
void matlabTest() {
    mtao::Grid<int,3> g(10,11,12);
    std::iota(g.begin(),g.end(),1);
    /*
    >> x = reshape(1:(10*11*12),[10,11,12]);
    >> [x(1,1,1),x(2,1,1),x(1,2,1),x(1,1,2),x(4,5,6)]

    ans =

         1     2    11   111   594

         */


    std::cout << g(0,0,0) << " " << 1 << std::endl;
    std::cout << g(1,0,0) << " " << 2 << std::endl;
    std::cout << g(0,1,0) << " " << 11 << std::endl;
    std::cout << g(0,0,1) << " " << 111 << std::endl;
    std::cout << g(3,4,5) << " " << 594 << std::endl;

}

int main(int argc, char * argv[]) {
    matlabTest();
    return 0;
    testA();
    testB();
}
