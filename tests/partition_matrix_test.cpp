#include <iostream>

#include "mtao/eigen/partition_vector.hpp"
int main(int argc, char *argv[]) {
    Eigen::VectorXi x(5);
    x << 0, 1, 2, 3, 4;
    {
        {
            auto [a, b, c] =
              mtao::eigen::partition_vector(x, std::tuple{ 0, 2, 3, 4 });
            a.setConstant(1);
            b.setConstant(2);
            c.setConstant(4);
        }

        std::cout << x.transpose() << std::endl;
        auto [a, b, c] = mtao::eigen::partition_vector(
          const_cast<const decltype(x) &>(x), std::tuple{ 0, 2, 3, 4 });

        std::cout << a.transpose() << std::endl;
        std::cout << b.transpose() << std::endl;
        std::cout << c.transpose() << std::endl;
    }
    {
        std::cout << "From sizes" << std::endl;
        {
            auto [a, b] =
              mtao::eigen::partition_vector_from_sizes(x, std::tuple{ 3, 2 });
            a.setConstant(1);
            b.setConstant(2);
        }

        auto [a, b] = mtao::eigen::partition_vector_from_sizes(
          const_cast<const decltype(x) &>(x), std::tuple{ 3, 2 });

        std::cout << a.transpose() << std::endl;
        std::cout << b.transpose() << std::endl;
        std::cout << x.transpose() << std::endl;
    }
    std::cout << "On a matrix" << std::endl;
    Eigen::MatrixXd A(10, 10);

    auto [r1, r2, r3] = mtao::eigen::partition_matrix(
      A, std::tuple{ 0, 3, 6, 10 }, std::tuple{ 0, 1, 2, 3, 10 });

    auto [c1, c2, c3, c4] = r1;
    auto [c5, c6, c7, c8] = r2;
    auto [c9, c10, c11, c12] = r3;

    c1.setConstant(1);
    c2.setConstant(2);
    c3.setConstant(3);
    c4.setConstant(4);
    c5.setConstant(5);
    c6.setConstant(6);
    c7.setConstant(7);
    c8.setConstant(8);
    c9.setConstant(9);
    c10.setConstant(10);
    c11.setConstant(11);
    c12.setConstant(12);

    {
        auto [r1, r2, r3] = mtao::eigen::partition_matrix_from_sizes(
          const_cast<const decltype(A) &>(A), std::tuple{ 3, 3, 4 }, std::tuple{ 1, 1, 1, 7 });

        auto [c1, c2, c3, c4] = r1;
        auto [c5, c6, c7, c8] = r2;
        auto [c9, c10, c11, c12] = r3;
        std::cout << c1 << "\n===\n";
        std::cout << c2 << "\n===\n";
        std::cout << c3 << "\n===\n";
        std::cout << c4 << "\n===\n";
        std::cout << c5 << "\n===\n";
        std::cout << c6 << "\n===\n";
        std::cout << c7 << "\n===\n";
        std::cout << c8 << "\n===\n";
        std::cout << c9 << "\n===\n";
        std::cout << c10 << "\n===\n";
        std::cout << c11 << "\n===\n";
    }
    std::cout << A << std::endl;
}
