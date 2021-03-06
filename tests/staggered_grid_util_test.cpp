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
    print(std::make_integer_sequence<size_t,std::tuple_size_v<decltype(a)>>(),a);

    auto printArr = [&](auto&& a) {

        std::copy(a.begin(),a.end(),std::ostream_iterator<int>(std::cout,","));
        std::cout << std::endl;
    };

    auto a00 = mtao::geometry::grid::staggered_grid::internal::offset_shape<D,0,0>(arr);
    auto aD0 = mtao::geometry::grid::staggered_grid::internal::offset_shape<D,D,0>(arr);
    auto a10 = mtao::geometry::grid::staggered_grid::internal::offset_shape<D,1,0>(arr);
    auto a11 = mtao::geometry::grid::staggered_grid::internal::offset_shape<D,1,1>(arr);
    std::cout << "Ar sizes: " << std::endl;
    printArr(a00);
    printArr(aD0);
    printArr(a10);
    printArr(a11);

    std::cout << std::endl << std::endl;

    //auto b = mtao::geometry::grid::staggered_grid::make_grids<mtao::geometry::grid::GridD<float,D>>(arr);
    //print(std::make_integer_sequence<size_t,std::tuple_size_v<decltype(b)>>(),b);
    for(int r = 0; r < D+1; ++r) {
        for(size_t l = 0; l  < combinatorial::nCr(D,r); ++l) {
            std::cout << " R,L:" << r<< "," << l << ": ";
            auto b = combinatorial::nCr_mask<D>(r,l);
            for(int i = 0; i < D; ++i) {
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
