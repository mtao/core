#include "mtao/geometry/mesh/earclipping.hpp"
#include "mtao/geometry/winding_number.hpp"
#include <iostream>
#include <numeric>

int main() {
    int N = 10;
    mtao::ColVectors<double,2> V(2,81);;
    mtao::ColVectors<double,81> VV(81,2);;
    VV << 0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,1.50836,1.31418,0.685818,0.685818,1.31418,1,1,1,1,0.491643,0.491643,1.50836,1,0.815328,1.18467,1.18467,0.815328,1,1.18467,0.815328,0.815328,1.18467,1,1.2988,1.2988,0.701195,0.701195,0.40239,0.516524,0.516524,0.516524,0.516524,1,0.701195,0.701195,1.2988,1.2988,1.59761,1.48348,1.48348,1.48348,1.48348,1,1,1,1,1.2988,0.701195,0.516524,0.516524,0.701195,1.2988,1.48348,1.48348,
0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,1,1.50836,1.50836,0.491643,0.491643,0.685818,1.31418,1.31418,0.685818,1,1,1,1.59761,1.48348,1.48348,1.48348,1.48348,0.40239,0.516524,0.516524,0.516524,0.516524,1,0.815328,1.18467,1.18467,0.815328,1,1.2988,1.2988,0.701195,0.701195,1,0.815328,1.18467,1.18467,0.815328,1,1.2988,1.2988,0.701195,0.701195,1.48348,1.48348,0.516524,0.516524,1,1,1.2988,0.701195,1,1,1.2988,0.701195;



    V = VV.transpose();
    std::cout << V << std::endl;
    std::vector<int> F(V.cols());
    std::iota(F.begin(),F.end(),0);
    F = {79,28,39,14,23,22,64};
    int NI=40, NJ=40;
    std::vector<std::vector<bool>> D(NI,std::vector<bool>(NJ,false));
    /*
    for(int i = 0; i < NI; ++i) {
        for(int j = 0; j < NJ; ++j) {
            mtao::Vec2d p(double(i)/(NI-1),-double(j)/(NJ-1));
            D[i][j] =  mtao::geometry::interior_winding_number(V,F,p);
        }
    }
    */
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


    auto print = [&](auto&& Fin, int sgna, int sgnb) {

        std::cout << "================================" << std::endl;
        std::cout << "================================" << std::endl;

        std::cout << V << std::endl;
        std::vector<std::vector<bool>> D(NI,std::vector<bool>(NJ,false));
        for(int i = 0; i < NI; ++i) {
            for(int j = 0; j < NJ; ++j) {
                mtao::Vec2d p(sgna * double(i)/(NI-1),sgnb*double(j)/(NJ-1));
                D[i][j] =  mtao::geometry::interior_winding_number(V,Fin,p);
            }
        }
        print_g(D);
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

        /*
        for(int i = 0; i < Fs.cols(); ++i) {
            auto f = Fs.col(i);
            std::array<int,3> F{{f(0),f(1),f(2)}};

            std::vector<std::vector<bool>> D(NI,std::vector<bool>(NJ,false));
            std::cout << std::endl;
            for(int i = 0; i < NI; ++i) {
                for(int j = 0; j < NJ; ++j) {
                    mtao::Vec2d p(sgna * double(i)/(NI-1),sgnb*double(j)/(NJ-1));
                    D[i][j] =  mtao::geometry::interior_winding_number(V,F,p);
                    if(D[i][j]) {
                        DD[i][j]++;
                    }
                }
                for(int i = 0; i < V.cols(); ++i) {
                    
                    if(f[0] != i && f[1] != i && f[2] != i) {
                        std::cout <<  mtao::geometry::winding_number(V,F,V.col(i)) << ",";
                    }
                }
                std::cout << std::endl;
            }
            print_g(D);
        }
        */
        print_gi(DD);
    };

    auto test = [&](int a, int b, int sgna, int sgnb) {
        V.row(0) =  sgna * VV.col(a);
        V.row(1) =  sgnb * VV.col(b);
        print(F,sgna,sgnb);
    };

    test(0,1,+1,+1);
    /*
    test(1,0,+1,+1);

    test(0,1,-1,+1);
    test(1,0,-1,+1);

    test(0,1,+1,-1);
    test(1,0,+1,-1);

    test(0,1,-1,-1);
    test(1,0,-1,-1);
    */

    return 0;
}

