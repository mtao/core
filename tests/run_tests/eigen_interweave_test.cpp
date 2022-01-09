#include <iostream>
#include "mtao/types.h"
#include "mtao/type_utils.h"
#include "mtao/eigen_utils.h"
int main() {
    using VecXf = mtao::Vector<float, Eigen::Dynamic>;
    using MatXf = mtao::SquareMatrix<float, Eigen::Dynamic>;
    using namespace mtao;
    using namespace mtao::eigen;

    int N = 10;
    VecXf A = VecXf::LinSpaced(N, 1, N);
    VecXf B = A.array() + N;

    MatXf C(N, 2);
    C.col(0) = A;
    C.col(1) = B;


    Eigen::Map<VecXf> CV(C.data(), C.size());
    std::cout << A.rows() << " " << A.cols() << std::endl;

    std::cout << "A [\n"
              << A.transpose() << "\n]" << std::endl;
    std::cout << "B [\n"
              << B.transpose() << "\n]" << std::endl;
    std::cout << "C [\n"
              << CV.transpose() << "\n]" << std::endl;
    std::cout << std::endl;

    C.col(0) = A.array() + 2 * N;
    C.col(1) = A.array() + 3 * N;

    std::cout << "C Splice [\n"
              << interweaveRows(A, B).transpose() << "\n]" << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "Hetero Splice (A,B,C) [\n"
              << interweaveRows(A, B, C).transpose() << "\n]" << std::endl;
    std::cout << interweaveRows(A, B, C).transpose() << "\n]" << std::endl;
    //std::cout << typeinfo( interweave(A,B,C).transpose() << "\n]"<< std::endl;
    std::cout << "C Splice [\n"
              << interweaveCols(A, B, A).transpose() << "\n]" << std::endl;
    std::cout << "C Splice [\n"
              << interweaveCols(Eigen::Vector3f::UnitX(), Eigen::Vector2f::UnitY(), Eigen::Vector3f::UnitZ()).transpose() << "\n]" << std::endl;
    std::cout << "C Splice [\n"
              << types::getTypeName(interweaveCols(Eigen::Vector3f::UnitX(), Eigen::Vector2f::UnitY(), Eigen::Vector3f::UnitZ())) << "\n]" << std::endl;
}
