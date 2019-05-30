#include <mtao/types.hpp>
#include <iostream>
#include <mtao/eigen/stl2eigen.hpp>
#include <array>
#include <set>
#include <vector>

int main() {
    using namespace mtao::eigen;
    using namespace mtao::types;
    
    auto func = [&](auto&& arr) {
    auto v = stl2eigen(arr);

    std::cout << getTypeName(arr) << ") => (" << getTypeName(v) << ")" << std::endl;
    std::cout << v.transpose() << std::endl;
    };
    func(std::array<int,4>{{1,2,3,4}});
    func(std::vector<double>{{1,2,3,4,5,6,7}});
    func(std::set<double>{{1,2,3,4,5,6,7}});
    {
        std::array<mtao::Vec2d,4> arr;
        mtao::vector<mtao::Vec2d> v;
        v.emplace_back(0.0,1.0);
        v.emplace_back(2.0,0.0);
        v.emplace_back(3.0,-.2);
        v.emplace_back(4.0,1.1);
        arr[0] = mtao::Vec2d(0.0,1.0);
        arr[1] = mtao::Vec2d(2.0,0.0);
        arr[2] = mtao::Vec2d(3.0,-.2);
        arr[3] = mtao::Vec2d(4.0,1.1);
        func(v);
        func(arr);
    }
    //func(std::vector<bool>{{1,0,0,0,1}});
}
