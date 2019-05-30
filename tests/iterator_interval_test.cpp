
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <set>

#include <mtao/iterator/interval.hpp>


using namespace mtao::iterator;

int main() {
    std::vector<int> a(30);
    std::iota(a.begin(),a.end(),0);
    for(auto [a,b]: interval<2>(a)) {
        std::cout << a << "," << b << std::endl;
    }
    for(auto [a,b,c]: interval<3>(a)) {
        std::cout << a << "," << b << "," << c << std::endl;
    }
    /*
    std::cout << "Cyclic!" << std::endl;
    for(auto [a,b]: cyclic_interval<2>(a)) {
        std::cout << a << "," << b  << std::endl;
    }
    */

}
