#include <iostream>
#include <spdlog/spdlog.h>
#include <mtao/solvers/linear/gauss_seidel.hpp>
#include <mtao/solvers/linear/jacobi.hpp>
#include <mtao/solvers/linear/preconditioned_conjugate_gradient.hpp>
#include <mtao/solvers/linear/conjugate_gradient.hpp>
#include <Eigen/Core>
#include <mtao/logging/timer.hpp>
#include <eigen3/unsupported/Eigen/SparseExtra>

int main(int argc, char *argv[]) {

    if (argc < 3) {
        std::cout << "eigen_market_problem_solver <A> <b>" << std::endl;
        return 1;
    }
    Eigen::SparseMatrix<double> A;
    Eigen::VectorXd b;

    Eigen::loadMarket(A, argv[1]);
    Eigen::loadMarketVector(b, argv[2]);

    std::cout << "A shape: " << A.rows() << " " << A.cols() << std::endl;
    std::cout << "B size: " << b.size() << std::endl;
    //std::cout << A << std::endl;

    std::cout << "Input norms: " << A.norm() << " " << b.norm() << std::endl;

    if constexpr (false) {
        auto t = mtao::logging::timer("Gauss seidel", true, "default", mtao::logging::Level::Info);
        auto x = mtao::solvers::linear::gauss_seidel(A, b);
        std::cout << x.norm() << " " << (A * x - b).norm() << std::endl;
    }
    if constexpr (false) {
        auto t = mtao::logging::timer("jacobi", true, "default", mtao::logging::Level::Info);
        auto x = mtao::solvers::linear::jacobi(A, b);
        std::cout << x.norm() << " " << (A * x - b).norm() << std::endl;
    }
    if constexpr (true) {
        auto t = mtao::logging::timer("MIC0PCG", true, "default", mtao::logging::Level::Info);
        mtao::VecXd x = b;
        x.setZero();
        mtao::solvers::linear::SparseCholeskyPCGSolve(A, b, x);
        std::cout << x.norm() << " " << (A * x - b).norm() << std::endl;
    }

    if constexpr (true) {
        auto t = mtao::logging::timer("CG", true, "default", mtao::logging::Level::Info);
        mtao::VecXd x = b;
        x.setZero();
        mtao::solvers::linear::CGSolve(A, b, x, 1e-8);
        std::cout << x.norm() << " " << (A * x - b).norm() << std::endl;
    }

    return 0;
}
