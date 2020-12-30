#include <iostream>
#include <mtao/iterator/range.hpp>

int main() {
    using namespace mtao::iterator;

    for (auto &&v : range(10)) {
        std::cout << v << ",";
    }
    for (auto &&v : range(0, -51, -2)) {
        std::cout << v << ",";
    }
    std::cout << std::endl;
}
