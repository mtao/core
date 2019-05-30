#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <set>

#include <mtao/iterator/reverse.hpp>


using namespace mtao::iterator;

int main() {
    std::vector<int> a(30);
    std::iota(a.begin(),a.end(),0);
    for(auto&& v: reverse(a)) {
        std::cout << v << std::endl;
    }

    return 0;
}
