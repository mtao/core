#include <iostream>
#include <iterator>
#include <array>
#include <mtao/geometry/grid/grid_utils.h>



int main(int argc, char * argv[]) {
    std::array<int,3> begin,end;
    std::fill(begin.begin(),begin.end(),5);
    end[0] = 10;
    end[1] = 11;
    end[2] = 12;

    //mtao::multi_loop(end,[](auto&& idx) {
    //        std::copy(idx.begin(),idx.end(),std::ostream_iterator<int>(std::cout," "));
    //        std::cout << std::endl;
    //        });
    mtao::geometry::grid::utils::multi_loop(begin,end,[](auto&& idx) {
            std::copy(idx.begin(),idx.end(),std::ostream_iterator<int>(std::cout," "));
            std::cout << std::endl;
            });
    mtao::geometry::grid::utils::multi_loop(std::array<int,2>{{2,3}},[](auto&& idx) {
            std::copy(idx.begin(),idx.end(),std::ostream_iterator<int>(std::cout," "));
            std::cout << std::endl;
            });
}
