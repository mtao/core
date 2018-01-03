#include <mtao/geometry/mesh/element_filter.hpp>




int main() {
    mtao::ColVectors<float,2> V(2,20);
    V.setRandom(2,20);
    mtao::ColVectors<int,3> T(3,20*19*18 / 6);

    std::cout << "V cols: " << V.cols() << std::endl;
    std::cout << "T cols: " << T.cols() << std::endl;

    int u = 0;
    for(int i = 0; i < V.cols()-2; ++i) {
        for(int j = i+1; j < V.cols()-1; ++j) {
            for(int k = j+1; k < V.cols(); ++k) {
                auto t = T.col(u++);
                t(0) = i;
                t(1) = j;
                t(2) = k;
            }
        }
    }
    std::cout << V << std::endl;
    std::cout << T << std::endl;
    auto f = [](auto v) -> bool {
        return v.dot(mtao::Vector<float,2>(1.0,1.0)) > 0;
    };

    auto T2 = mtao::geometry::mesh::element_filter(V,T,f);
    std::cout << T2 << std::endl;


}


