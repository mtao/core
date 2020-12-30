#include <iostream>
#include "mtao/types.h"
#include "mtao/type_utils.h"
#include "mtao/eigen_utils.h"
int main() {
    using VecXf = mtao::Vector<float, Eigen::Dynamic>;
    //using MatXf = mtao::SquareMatrix<float,Eigen::Dynamic>;
    using namespace mtao;
    using namespace mtao::eigen;

    int N = 10;
    VecXf A = VecXf::LinSpaced(N, 1, N);
    VecXf B = A.array() + N;
    VecXf C = A.array() + 2 * N;

    std::cout << "A [\n"
              << A.transpose() << "\n]" << std::endl;
    std::cout << "B [\n"
              << B.transpose() << "\n]" << std::endl;
    std::cout << "C [\n"
              << C.transpose() << "\n]" << std::endl;


    std::cout << "C Splice [\n"
              << vstack(A, B).transpose() << "\n]" << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "Hetero Splice (A,B,C) [\n"
              << vstack(A, B, C).transpose() << "\n]" << std::endl;
    std::cout << vstack(A, B, C).transpose() << "\n]" << std::endl;
    //std::cout << typeinfo( interweave(A,B,C).transpose() << "\n]"<< std::endl;
    std::cout << "C Splice [\n"
              << hstack(A, B, A).transpose() << "\n]" << std::endl;
    std::cout << "C Splice [\n"
              << hstack(Eigen::Vector3f::UnitX(), Eigen::Vector2f::UnitY(), Eigen::Vector3f::UnitZ()).transpose() << "\n]" << std::endl;
    std::cout << "C Splice [\n"
              << types::getTypeName(hstack(Eigen::Vector3f::UnitX(), Eigen::Vector2f::UnitY(), Eigen::Vector3f::UnitZ())) << "\n]" << std::endl;
}
