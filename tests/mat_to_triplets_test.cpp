#include <mtao/eigen/mat_to_triplets.hpp>
#include <iostream>




int main(int argc, char * argv[]) {


    {
        Eigen::SparseMatrix<double> A(3,3);
        A.coeffRef(0,2) = 3;
        A.coeffRef(1,0) = 4;
        A.coeffRef(2,0) = 5;


        auto trips = mtao::eigen::mat_to_triplets(A);
        for(auto&& t: trips) {
            std::cout << t.row() <<"," << t.col() << ":" << t.value() << " ";
        }
        std::cout << std::endl;

    }
    {
        Eigen::MatrixXf A(4,4);
        A.setConstant(-.1);
        A.coeffRef(0,2) = 3;
        A.coeffRef(1,0) = 4;
        A.coeffRef(2,0) = 5;


        auto trips = mtao::eigen::mat_to_triplets(A,.12);
        for(auto&& t: trips) {
            std::cout << t.row() <<"," << t.col() << ":" << t.value() << " ";
        }
        std::cout << std::endl;

    }

}
