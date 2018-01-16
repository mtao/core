#include <mtao/eigen/slice_filter.hpp>
#include <iostream>




int main() {

    int S = 10;
    mtao::ColVectors<int,3> T(3,S);

    std::cout << "T cols: " << T.cols() << std::endl;
    for(int i = 0; i < S; ++i) {
        T.col(i) = mtao::Vector<int,3>(3*i,3*i+1,3*i+2);
    }
    std::cout << T << std::endl << std::endl;

    auto T2 = mtao::eigen::slice_filter_col(T,mtao::VectorX<bool>::Random(T.cols()));
    std::cout << T2 << std::endl << std::endl;
    auto T3 = mtao::eigen::slice_filter_row(T,mtao::VectorX<bool>::Random(T.rows()));
    std::cout << T3.rows() << " " << T3.cols() << std::endl;
    std::cout << T3 << std::endl << std::endl;


}


