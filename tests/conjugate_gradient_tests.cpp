
#include <iostream>
#include <spdlog/spdlog.h>
#include <mtao/solvers/linear/preconditioned_conjugate_gradient.hpp>
#include <mtao/solvers/linear/conjugate_gradient.hpp>
#include <catch2/catch.hpp>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <mtao/logging/json_sink.hpp>

using E = std::array<int, 2>;


TEST_CASE("Diagonal PCG", "[pcg,linear,cholesky]") {

    Eigen::MatrixXd A(10, 10);
    A.setIdentity();

    std::cout << A << std::endl;
    mtao::VecXd b = mtao::VecXd::Random(10);
    std::cout << "===\n"
              << b.transpose() << std::endl;

    mtao::VecXd x(b);
    x.setZero();
    std::cout << "===\n"
              << x.transpose() << std::endl;
    mtao::solvers::linear::CholeskyPCGSolve(A, b, x);

    REQUIRE((x - b).norm() == Approx(0));
}
TEST_CASE("Random PCG", "[pcg,linear,cholesky]") {

    Eigen::MatrixXd A(10, 10);
    A.setRandom();
    A = A * A.transpose();
    A.diagonal().array() += 10;

    std::cout << A << std::endl;
    mtao::VecXd b = mtao::VecXd::Random(10);

    mtao::VecXd x(b);
    x.setZero();
    mtao::solvers::linear::CholeskyPCGSolve(A, b, x);


    REQUIRE((A * x - b).norm() < 1e-5);
}
TEST_CASE("Diagonal Sparse PCG", "[pcg,linear,cholesky]") {

    Eigen::MatrixXd _A(10, 10);
    _A.setIdentity();

    Eigen::SparseMatrix<double> A = _A.sparseView();

    std::cout << A << std::endl;
    mtao::VecXd b = mtao::VecXd::Random(10);
    std::cout << "===\n"
              << b.transpose() << std::endl;

    mtao::VecXd x(b);
    x.setZero();
    std::cout << "===\n"
              << x.transpose() << std::endl;
    mtao::solvers::linear::CholeskyPCGSolve(A, b, x);

    REQUIRE((x - b).norm() == Approx(0));
}
TEST_CASE("Random Spasre PCG", "[pcg,linear,cholesky]") {

    Eigen::MatrixXd _A(10, 10);
    _A.setRandom();
    _A = _A * _A.transpose();
    _A.diagonal().array() += 10;

    Eigen::SparseMatrix<double> A = _A.sparseView();

    std::cout << A << std::endl;
    mtao::VecXd b = mtao::VecXd::Random(10);

    mtao::VecXd x(b);
    x.setZero();
    mtao::solvers::linear::CholeskyPCGSolve(A, b, x);


    REQUIRE((A * x - b).norm() < 1e-5);
}

TEST_CASE("Random CG With Log", "[log,cg,linear,cholesky]") {

    Eigen::MatrixXd A(10, 10);
    A.setRandom();
    A = A * A.transpose();
    A.diagonal().array() += 10;

    std::cout << A << std::endl;
    mtao::VecXd b = mtao::VecXd::Random(10);

    mtao::VecXd x(b);
    x.setZero();

    spdlog::info("Logger time!");
    auto logger = mtao::logging::make_json_file_logger("cholpcgsolve", "cholpgslog.log", true);

    mtao::solvers::linear::CGSolve(A, b, x, 1e-8, logger);

    REQUIRE((A * x - b).norm() < 1e-5);
}
