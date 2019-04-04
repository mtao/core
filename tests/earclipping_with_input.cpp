#include "mtao/geometry/mesh/earclipping.hpp"
#include "mtao/geometry/winding_number.hpp"
#include <iostream>
#include <numeric>

int main() {
    int N = 10;
    mtao::ColVectors<double,2> V(2,7);;
    mtao::ColVectors<double,7> VV(7,2);;
    VV << -0.15    , -0.242705,
      -0.255195    , -0.157719,
      -0.3  ,     0,
      -0.502 , 0,
      -0.502 , -0.502,
      0, -0.502,
      0 ,   -0.3;
    V = VV.transpose();
    V.col(0) = VV.row(1);
    V.col(1) = VV.row(0);
    std::cout << V << std::endl;
    std::vector<int> F(V.cols());
    std::iota(F.begin(),F.end(),0);
    int NI=40, NJ=40;
    std::vector<std::vector<bool>> D(NI,std::vector<bool>(NJ,false));
    for(int i = 0; i < NI; ++i) {
        for(int j = 0; j < NJ; ++j) {
            mtao::Vec2d p(-double(i)/(NI-1),-double(j)/(NJ-1));
            D[i][j] =  mtao::geometry::interior_winding_number(V,F,p);
        }
    }
    auto print_g = [&](auto&& g) {
        for(auto&& vj: g) {
        for(auto&& v: vj) {
                std::cout << (v?'o':'.') << " ";
        }
            std::cout << std::endl;
        }
    };
    auto print_gi = [&](auto&& g) {
        for(auto&& vj: g) {
        for(auto&& v: vj) {
                std::cout << v;
        }
            std::cout << std::endl;
        }
    };
    print_g(D);


    auto print = [&](auto&& Fin) {
        /*
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
        */
        auto Fs = mtao::geometry::mesh::earclipping(V,Fin);
        std::vector<std::vector<int>> DD(NI,std::vector<int>(NJ,0));
        
        for(int i = 0; i < V.cols(); ++i) {
            auto f = V.col(i);
            std::cout << "v " << f.transpose() << std::endl;
        }
        for(int i = 0; i < Fs.cols(); ++i) {
            auto f = Fs.col(i);
            std::cout << "f " << (f.array()+1).transpose() << std::endl;
        }

        for(int i = 0; i < Fs.cols(); ++i) {
            auto f = Fs.col(i);
            std::array<int,3> F{{f(0),f(1),f(2)}};

            std::vector<std::vector<bool>> D(NI,std::vector<bool>(NJ,false));
            std::cout << std::endl;
            for(int i = 0; i < NI; ++i) {
                for(int j = 0; j < NJ; ++j) {
                    mtao::Vec2d p(-double(i)/(NI-1),-double(j)/(NJ-1));
                    D[i][j] =  mtao::geometry::interior_winding_number(V,F,p);
                    if(D[i][j]) {
                        DD[i][j]++;
                    }
                }
            }
            print_g(D);
        }
        print_gi(DD);
    };


    print(F);
    V = VV.transpose();
    print(F);
    return 0;
}

