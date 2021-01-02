#include <iostream>
#include <mtao/solvers/linear/jacobi.hpp>
#include <mtao/solvers/linear/gauss_seidel.hpp>
#include <catch2/catch.hpp>


TEST_CASE("Jacobi", "[linear,solver]") {
    mtao::MatXd A(5, 5);
    A.setRandom();
    A = A.transpose() * A;
    A.diagonal().array() += 1.;


    mtao::VecXd x(A.cols());
    x.setRandom();

    mtao::VecXd b = A * x;

    mtao::VecXd xx = mtao::solvers::linear::jacobi(A, b);

    std::cout << A << std::endl;
    std::cout << "b: " << b.transpose() << std::endl;
    std::cout << "x: " << x.transpose() << std::endl;
    std::cout << xx.transpose() << std::endl;
    REQUIRE(x.isApprox(xx, 1e-4));
}
TEST_CASE("Gauss Seidel", "[linear,solver]") {
    mtao::MatXd A(5, 5);
    A.setRandom();
    A = A.transpose() * A;
    A.diagonal().array() += 1.;


    mtao::VecXd x(A.cols());
    x.setRandom();

    mtao::VecXd b = A * x;

    mtao::VecXd xx = mtao::solvers::linear::gauss_seidel(A, b);
    std::cout << x.transpose() << std::endl;
    std::cout << xx.transpose() << std::endl;

    REQUIRE(x.isApprox(xx, 1e-4));
}
