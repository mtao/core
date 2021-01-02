
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <set>

#include <mtao/iterator/interval.hpp>


using namespace mtao::iterator;

int main() {
    std::vector<size_t> a(30);
    std::iota(a.begin(), a.end(), 0);
    for (auto [a, b] : interval<2>(a)) {
        std::cout << a << "," << b << std::endl;
    }
    for (auto [a, b, c] : interval<3>(a)) {
        std::cout << a << "," << b << "," << c << std::endl;
    }
    std::cout << "Cyclic!" << std::endl;
    for (auto tup : cyclic_interval<4>(a)) {
        fmt::print("{}\n", tup);
    }

    for (auto [a, b] : interval<2>(a)) {
        a = 2;
    }
    for (auto &&v : a) {
        std::cout << v << std::endl;
    }


    std::iota(a.begin(), a.end(), 0);
    for (auto &&v : a) {
        std::cout << v << std::endl;
    }
    for (auto [u, v] : cyclic_interval<2>(a)) {
        u = 3;
    }

    for (auto &&v : a) {
        std::cout << v << std::endl;
    }
}
