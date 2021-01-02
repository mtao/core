#include "mtao/geometry/mesh/earclipping.hpp"
#include "mtao/geometry/winding_number.hpp"
#include <iostream>
#include <numeric>


int main() {
    int N = 10;
    mtao::ColVectors<double, 2> V(2, 3 + N);
    V.col(0) = mtao::Vec2d(0.0, 0.0);
    V.col(1) = mtao::Vec2d(1.0, 0.0);
    for (int i = 0; i <= N; ++i) {
        V.col(i + 2) = mtao::Vec2d(std::cos(M_PI * i / double(N)), -std::sin(M_PI * i / double(N))) * .5 + mtao::Vec2d(.5, 1);
    }
    std::vector<int> F(V.cols());
    std::iota(F.begin(), F.end(), 0);
    int NI = 40, NJ = 40;
    std::vector<std::vector<bool>> D(NI, std::vector<bool>(NJ, false));
    for (int i = 0; i < NI; ++i) {
        for (int j = 0; j < NJ; ++j) {
            mtao::Vec2d p(double(i) / (NI - 1), double(j) / (NJ - 1));
            D[i][j] = mtao::geometry::interior_winding_number(V, F, p);
        }
    }
    auto print_g = [&](auto &&g) {
        for (auto &&vj : g) {
            for (auto &&v : vj) {
                std::cout << (v ? 'o' : '.') << " ";
            }
            std::cout << std::endl;
        }
    };
    //print_g(D);


    auto print = [&](auto &&Fin) {
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
        auto Fs = mtao::geometry::mesh::earclipping(V, Fin);

        for (int i = 0; i < V.cols(); ++i) {
            auto f = V.col(i);
            std::cout << "v " << f.transpose() << std::endl;
        }
        for (int i = 0; i < Fs.cols(); ++i) {
            auto f = Fs.col(i);
            std::cout << "f " << (f.array() + 1).transpose() << std::endl;
        }

        for (int i = 0; i < Fs.cols(); ++i) {
            auto f = Fs.col(i);
            std::array<int, 3> F{ { f(0), f(1), f(2) } };

            std::vector<std::vector<bool>> D(NI, std::vector<bool>(NJ, false));
            for (int i = 0; i < NI; ++i) {
                for (int j = 0; j < NJ; ++j) {
                    mtao::Vec2d p(double(i) / (NI - 1), double(j) / (NJ - 1));
                    D[i][j] = mtao::geometry::interior_winding_number(V, F, p);
                }
                std::cout << std::endl;
            }
            print_g(D);
        }
    };


    print(F);
    print(std::vector<int>{ 0, 1, 2, 3 });
    std::iota(F.rbegin(), F.rend(), 0);
    print(F);
    //print(std::vector<int>{F,F});
    return 0;
}
