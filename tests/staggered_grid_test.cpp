#include <mtao/geometry/grid/staggered_grid.hpp>
#include <iostream>
#include <numeric>
#include <iterator>
#include <array>

using namespace mtao;
template <size_t... N, typename... Args>
void print(std::integer_sequence<size_t,N...>, const std::tuple<Args...>& arr) {

    auto p = [](auto&& a) {
        std::copy(a.begin(),a.end(),std::ostream_iterator<int>(std::cout,","));
        std::cout << std::endl;
    };
    ( p(std::get<N>(arr)),...);
}

template <int D>
void test() {
    std::cout << "Test" << std::endl;
    std::array<int,D> arr;
    std::iota(arr.begin(),arr.end(),10);
    for(auto&& a: arr) {
        std::cout << a << ",";
    }
    std::cout << std::endl;
    auto a = mtao::geometry::grid::staggered_grid::staggered_grid_sizes(arr);

    //auto a = std::make_tuple(
    //        std::array<int,2>{{3,4}},
    //        std::array<int,3>{{2,3,4}}
    //        );
    print(std::make_integer_sequence<size_t,std::tuple_size_v<decltype(a)>>(),a);
}

int main() {
    test<2>();
    test<3>();
    return 0;
}
