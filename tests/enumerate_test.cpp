#include <iostream>
#include <vector>
#include <mtao/enumerate.hpp>


int main() {
    for(auto&& [i,v]: mtao::enumerate({2,3,4,5})) {
        std::cout << i << ") " << v << std::endl;
    }
    std::vector<int> vec = {4,5,1,2};
    for(auto&& [i,v]: mtao::enumerate(vec)) {
        std::cout << i << ") " << v << std::endl;
    }
    for(auto&& [i,v]: mtao::enumerate(vec)) {
        v = i + v;
    }
    std::cout << "transformed" << std::endl;
    for(auto&& [i,v]: mtao::enumerate(vec)) {
        std::cout << i << ") " << v << std::endl;
    }
}

