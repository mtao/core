#include "mtao/geometry/mesh/earclipping.hpp"
#include "mtao/geometry/winding_number.hpp"
#include <iostream>


int main() {
    mtao::ColVectors<double,2> V(2,5);
    V.col(0) = mtao::Vec2d(0.0,0.0);
    V.col(1) = mtao::Vec2d(1.0,0.0);
    V.col(2) = mtao::Vec2d(1.0,1.0);
    V.col(3) = mtao::Vec2d(0.0,1.0);
    V.col(4) = mtao::Vec2d(0.5,0.5);
    std::vector<int> F = {0,1,2,3,4};
    size_t NI=10, NJ=10;
    std::vector<std::vector<bool>> D(NI,std::vector<bool>(NJ,false));
    for(int i = 0; i < NI; ++i) {
        for(int j = 0; j < NJ; ++j) {
            mtao::Vec2d p(double(i)/(NI-1),double(j)/(NJ-1));
            D[i][j] =  mtao::geometry::interior_winding_number(V,F,p);
             std::cout <<  mtao::geometry::winding_number(V,F,p) << " ";
        }
        std::cout << std::endl;
    }
    auto print_g = [&](auto&& g) {
        for(auto&& vj: g) {
        for(auto&& v: vj) {
                std::cout << (v?'o':'.') << " ";
        }
            std::cout << std::endl;
        }
    };
    print_g(D);
    F = {4,3,2,1,0};
    for(int i = 0; i < NI; ++i) {
        for(int j = 0; j < NJ; ++j) {
            mtao::Vec2d p(double(i)/(NI-1),double(j)/(NJ-1));
            D[i][j] =  mtao::geometry::interior_winding_number(V,F,p);
             std::cout <<  mtao::geometry::winding_number(V,F,p) << " ";
        }
        std::cout << std::endl;
    }

    print_g(D);

    auto print = [&](auto&& Fin) {
        std::cout << "================================" << std::endl;
        std::cout << "================================" << std::endl;

        std::vector<std::vector<bool>> D(NI,std::vector<bool>(NJ,false));
        for(int i = 0; i < NI; ++i) {
            for(int j = 0; j < NJ; ++j) {
                mtao::Vec2d p(double(i)/(NI-1),double(j)/(NJ-1));
                D[i][j] =  mtao::geometry::interior_winding_number(V,Fin,p);
            }
            std::cout << std::endl;
        }
        print_g(D);
        auto Fs = mtao::geometry::mesh::earclipping(V,Fin);
        for(int i = 0; i < Fs.cols(); ++i) {
            auto f = Fs.col(i);
            std::array<int,3> F{{f(0),f(1),f(2)}};

            std::vector<std::vector<bool>> D(NI,std::vector<bool>(NJ,false));
            for(int i = 0; i < NI; ++i) {
                for(int j = 0; j < NJ; ++j) {
                    mtao::Vec2d p(double(i)/(NI-1),double(j)/(NJ-1));
                    D[i][j] =  mtao::geometry::interior_winding_number(V,F,p);
                }
                std::cout << std::endl;
            }
            print_g(D);
        }
    };

    print(F);
    print(std::vector<int>{0,1,2,3});
    print(std::vector<int>{4,3,2,1,0});
    //print(std::vector<int>{F,F});
    return 0;
}
