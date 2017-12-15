#include <iostream>
#include <iterator>
#include "mtao/static_iteration.h"
#include <array>

template <int I>
struct Square {
    constexpr static int input = I;
    constexpr static int output = I * I;
};


struct PerDimTest {
    using value_type = int;
    template <int D>
    int get() const {
        return Square<D>::output;
    }
};

int main(int argc, char * argv[]) {
    PerDimTest pdt;
    mtao::static_loop_by_value<5>([](int,int input) {
            std::cout << input << std::endl;
            },pdt);
}
