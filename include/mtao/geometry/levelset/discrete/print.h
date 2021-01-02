#ifndef DISCRETE_PRINT_H
#define DISCRETE_PRINT_H
#include <iostream>
#include "../discrete.h"


namespace levelset {
namespace discrete {
    template<typename T, int D>
    void print_grid(const mtao::Grid<T, D> &g) {
    }
    template<typename T>
    void print_grid(const mtao::Grid<T, 2> &g) {
        for (int i = 0; i < g.template width<0>(); ++i) {
            for (int j = 0; j < g.template width<1>(); ++j) {
                std::cout << (g(i, j) < 0 ? "+" : ".");
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

}// namespace discrete
}// namespace levelset


#endif//DISCRETE_PRINT_H
