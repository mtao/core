#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <set>

#include <mtao/iterator/cycle.hpp>


using namespace mtao::iterator;

int main() {
    std::vector<int> a(30);
    std::iota(a.begin(),a.end(),0);

    int cnt = 0;
    for(auto&& v: cycle(a)) {
        if(cnt > 45) {
            return 0;
        }
        std::cout << cnt << ": " << v << std::endl;
        cnt++;
    }

}
