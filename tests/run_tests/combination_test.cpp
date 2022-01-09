#include <mtao/algebra/combinatorial.hpp>
#include <iostream>
#include <numeric>
#include <tuple>
#include <iterator>
#include <array>

using namespace mtao;

template<int D>
void test() {
    std::cout << "Test" << std::endl;
    std::array<int, D> arr;
    std::iota(arr.begin(), arr.end(), 10);

    for (int r = 0; r < D + 1; ++r) {
        for (size_t l = 0; l < combinatorial::nCr(D, r); ++l) {
            std::cout << " R,L:" << r << "," << l << ": ";
            auto b = combinatorial::nCr_mask<D>(r, l);
            for (int i = 0; i < D; ++i) {
                std::cout << int(b[i]);
            }
            std::cout << std::endl;
        }
    }
}

int main() {
    test<2>();
    test<3>();
    return 0;
}
