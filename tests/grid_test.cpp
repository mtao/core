#include <iostream>
#include <mtao/geometry/grid/grid.h>
#include <utility>
#include <numeric>




template <int D>
void test() {
    using Grid = mtao::geometry::grid::GridD<float,D>;
    typename Grid::coord_type a;
    std::iota(a.begin(),a.end(),3);
    Grid g(a);
    auto bb = g.bbox();
    std::cout << bb.min().transpose() << " => " << bb.max().transpose() << std::endl;

    mtao::geometry::grid::utils::multi_loop(g.shape(),[&](auto&& a) {
            std::cout << g.vertex(a).transpose() << std::endl;
            });

    std::cout << g.vertices() << std::endl;
    std::cout << g.local_vertices() << std::endl;
    for(int i = 0; i < D; ++i) {
        std::cout << 0.5 << " => " << g.world_coord(.5,i) << " => " << g.local_coord(g.world_coord(.5,i),i) << std::endl;
    }

}


int main(int argc, char * argv[]) {
    test<2>();
    test<3>();
}
