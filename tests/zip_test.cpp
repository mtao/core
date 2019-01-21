#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <set>

#include <mtao/iterator/zip.hpp>
#include <mtao/iterator/range.hpp>
#include <mtao/iterator/enumerate.hpp>



int main() {
    std::vector<int> a(30);
    std::iota(a.begin(),a.end(),0);
    std::vector<int> b(45);
    std::iota(b.begin(),b.end(),-100);

    std::set<double> c;
    std::transform(a.begin(),a.end(),std::inserter(c,c.end()),[](int v) -> double { return v / 2.32; });

    for(auto&& v: a) { std::cout<<v << "," ;}std::cout<< std::endl;
    for(auto&& v: b) { std::cout<<v << "," ;}std::cout<< std::endl;
    std::cout << "meh" << std::endl;

    int x[3] = {1,2,3};
    using namespace mtao::iterator;

    std::cout << "Array" << std::endl;
    for(auto&& [u,v]: zip(x,x)) {
        std::cout << u << "," << v << std::endl;
    }


    for(auto [u,v,w]: zip(a,b,c)) {
        std::cout << u << "," << v << ": " << w << std::endl;
    }

    std::cout << "meh" << std::endl;
    for(auto [u,vw]: zip(a,zip(b,c))) {
        auto [v,w] = vw;
        std::cout << u << "," << v << ": " << w << std::endl;

    }

    std::cout << "A values should change after this one" << std::endl;
    for(auto&& [i,v]: enumerate(a)) {
        std::cout << i << ": " << v << "==>";
        v = -i;
        std::cout << v << std::endl;
    }
    for(auto&& v: a) { std::cout<<v << "," ;}std::cout<< std::endl;
    std::cout << "Check a values?" << std::endl;
    for(auto [i,v]: enumerate(a)) {
        std::cout << i << ": " << v;
    std::cout << std::endl;
    }
    for(auto&& [i,uvw]: enumerate(zip(a,b,c))) {
        auto&& [u,v,w] = uvw;
        std::cout << i << ": " << u << "," << v << ":" << w  << std::endl;
    }

    for(auto&& v: range(0,-51,-2)) {
        std::cout << v << ",";
    }
    std::cout << std::endl;

}
